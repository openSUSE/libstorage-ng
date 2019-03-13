/*
 * Copyright (c) [2016-2019] SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#include <regex>

#include "storage/Utils/Algorithm.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Holders/User.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Prober.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Storage.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    // TODO handle attach and detach
    // TODO synchronization after actions
    // TODO write sequential_cutoff and writeback_percent persistently to survive reboot


    const char* DeviceTraits<Bcache>::classname = "Bcache";

    const vector<string> EnumTraits<BcacheType>::names({
	"backed", "flash-only"
    });

    const vector<string> EnumTraits<CacheMode>::names({
	"writethrough", "writeback", "writearound", "none"
    });

    Bcache::Impl::Impl(const string& name, BcacheType type)
	: Partitionable::Impl(name), cache_mode(CacheMode::NONE),
	  sequential_cutoff(0), type(type), writeback_percent(0)
    {
	if(get_type() == BcacheType::BACKED)
	{
	    cache_mode = CacheMode::WRITETHROUGH;
	    sequential_cutoff = 8 * MiB;
	    writeback_percent = 10;
	}

	update_sysfs_name_and_path();
    }


    Bcache::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), cache_mode(CacheMode::NONE),
          sequential_cutoff(0), type(BcacheType::BACKED), writeback_percent(0)
    {
	string tmp;

	if(getChildValue(node, "type", tmp))
	    type = toValueWithFallback(tmp, BcacheType::BACKED);

	if(get_type() == BcacheType::BACKED)
	{
	    cache_mode = CacheMode::WRITETHROUGH;

	    if(getChildValue(node, "cache-mode", tmp))
		cache_mode = toValueWithFallback(tmp, CacheMode::WRITETHROUGH);

	    getChildValue(node, "writeback-percent", writeback_percent);
	    getChildValue(node, "sequential-cutoff", sequential_cutoff);
	}
    }


    string
    Bcache::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Bcache").translated;
    }


    string
    Bcache::Impl::get_sort_key() const
    {
	static const vector<NameSchema> name_schemata = {
	    NameSchema(regex(DEV_DIR "/bcache([0-9]+)", regex::extended), { { 3, '0' } }),
	};

	return format_to_name_schemata(get_name(), name_schemata);
    }


    void
    Bcache::Impl::reassign_numbers(Devicegraph* devicegraph)
    {
	// not many bcache devices expected (<10), so lets just do tail-recursive
	// algorithm. It finds free name and ensure that all devices
	// with higher number are not in memory. And if there is a memory one,
	// then rename it to free name and run again. It always terminate as each step remove one
	// in-memory bcache that has hole before till all in-memory caches occupy the lowest
	// available numbers.
	const string free_name = find_free_name(devicegraph);
	unsigned free_number = device_to_name_and_number(free_name).second;

	vector<Bcache*> bcaches = get_all(devicegraph);

	sort(bcaches.begin(), bcaches.end(), compare_by_number);

	for(Bcache* bcache: bcaches)
	{
	    if (bcache->get_number() > free_number && !bcache->exists_in_probed())
	    {
		bcache->get_impl().set_number(free_number);
		reassign_numbers(devicegraph);
		return;
	    }
	}
    }


    string
    Bcache::Impl::find_free_name(const Devicegraph* devicegraph)
    {
	vector<const Bcache*> bcaches = get_all(devicegraph);

	sort(bcaches.begin(), bcaches.end(), compare_by_number);

	unsigned int free_number = first_missing_number(bcaches, 0);

	return DEV_DIR "/bcache" + to_string(free_number);
    }


    void
    Bcache::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValue(node, "type", toString(type));

	if(get_type() == BcacheType::BACKED)
	{
	    setChildValue(node, "cache-mode", toString(cache_mode));
	    setChildValue(node, "writeback-percent", writeback_percent);
	    setChildValue(node, "sequential-cutoff", sequential_cutoff);
	}
    }


    ResizeInfo
    Bcache::Impl::detect_resize_info() const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    bool
    Bcache::Impl::is_valid_name(const string& name)
    {
	static regex name_regex(DEV_DIR "/bcache[0-9]+", regex::extended);

	return regex_match(name, name_regex);
    }


    static bool
    is_backed(Prober& prober, const string& short_name)
    {
	SystemInfo& system_info = prober.get_system_info();

	string dev_path = SYSFS_DIR "/devices/virtual/block/" + short_name + "/dev";
	const File& dev_file = system_info.getFile(dev_path);
	string majorminor = dev_file.get<string>();

	string backing_dev_path = SYSFS_DIR "/devices/virtual/block/" + short_name + "/bcache/../dev";
	const File& backing_dev_file = system_info.getFile(backing_dev_path);
	string backing_majorminor = backing_dev_file.get<string>();

	return majorminor != backing_majorminor;
    }


    void
    Bcache::Impl::probe_bcaches(Prober& prober)
    {
	for (const string& short_name : prober.get_sys_block_entries().bcaches)
	{
	    string name = DEV_DIR "/" + short_name;

	    try
	    {
		BcacheType type = is_backed(prober, short_name) ? BcacheType::BACKED : BcacheType::FLASH_ONLY;

		Bcache* bcache = Bcache::create(prober.get_system(), name, type);
		bcache->get_impl().probe_pass_1a(prober);
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		error_callback(prober.get_probe_callbacks(), sformat(_("Probing bcache %s failed"),
								     name), exception);
	    }
	}
    }


    static CacheMode
    parse_mode(const File& cache_mode_file)
    {
	string modes = cache_mode_file.get<string>();
	regex rgx("\\[(.*)\\]");
	smatch match;
	if ( regex_search(modes, match, rgx) )
	{
	    return toValueWithFallback(match[1], CacheMode::WRITETHROUGH);
	}
	else
	{
	    y2err("Cannot parse cache_mode file: '" << modes << "'");
	    return CacheMode::WRITETHROUGH;
	}
    }


    // mapping between human string of libstorage-ng and bcache sysfs sizes
    static const map<std::string, unsigned long long> size_mapping = {
	{ "k", KiB },
	{ "M", MiB },
	{ "G", GiB }
    };


    static unsigned long long
    parse_size(const File& file)
    {
	string size_s = file.get<string>();
	regex rgx("(\\d+\\.?\\d*)([kMG])?");
	smatch match;
	unsigned long long result = 0;
	if ( regex_search(size_s, match, rgx) )
	{
	    unsigned long long multi = 1;
	    auto unit = size_mapping.find(match[2]);
	    if (unit != size_mapping.end())
		multi = unit->second;

	    result = stof(match[1]) * multi;
	}
	else
	{
	    y2err("Cannot parse size '" << size_s << "'");
	}

	return result;
    }


    void
    Bcache::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	SystemInfo& system_info = prober.get_system_info();

	const File& size_file = get_sysfs_file(system_info, "size");
	set_region(Region(0, size_file.get<unsigned long long>(), 512));

	if(get_type() == BcacheType::BACKED)
	{
	    const File& cache_mode_file = get_sysfs_file(system_info, "bcache/cache_mode");
	    set_cache_mode(parse_mode(cache_mode_file));

	    const File& writeback_percent_file = get_sysfs_file(system_info, "bcache/writeback_percent");
	    set_writeback_percent(writeback_percent_file.get<unsigned>());

	    const File& sequential_cutoff_file = get_sysfs_file(system_info, + "bcache/sequential_cutoff");
	    set_sequential_cutoff(parse_size(sequential_cutoff_file));
	}
    }


    void
    Bcache::Impl::probe_pass_1b(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1b(prober);

	if (get_type() == BcacheType::BACKED)
	{
	    SystemInfo& system_info = prober.get_system_info();

	    // Creating relationship with its backing device
	    const File& dev_file = get_sysfs_file(system_info, "bcache/../dev");
	    string dev = DEV_DIR "/block/" + dev_file.get<string>();

	    prober.add_holder(dev, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
		User::create(system, a, b);
	    });
	}
    }


    uint64_t
    Bcache::Impl::used_features() const
    {
	return UF_BCACHE | Partitionable::Impl::used_features();
    }


    unsigned int
    Bcache::Impl::get_number() const
    {
	return device_to_name_and_number(get_name()).second;
    }


    void
    Bcache::Impl::set_number(unsigned int number)
    {
	std::pair<string, unsigned int> pair = device_to_name_and_number(get_name());

	set_name(name_and_number_to_device(pair.first, number));

	update_sysfs_name_and_path();
    }


    const BlkDevice*
    Bcache::Impl::get_backing_device() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BlkDevice*> ret = devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));

	if(ret.empty())
	    ST_THROW(DeviceNotFound("No backing device"));

	return ret.front();
    }


    bool
    Bcache::Impl::has_bcache_cset() const
    {
	// TODO, write some generic helper

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BcacheCset*> ret = devicegraph.filter_devices_of_type<const BcacheCset>(devicegraph.parents(vertex));

	return !ret.empty();
    }


    const BcacheCset*
    Bcache::Impl::get_bcache_cset() const
    {
	// TODO, write some generic helper

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BcacheCset*> ret = devicegraph.filter_devices_of_type<const BcacheCset>(devicegraph.parents(vertex));

	return ret.front();
    }


    void
    Bcache::Impl::add_bcache_cset(BcacheCset* bcache_cset)
    {
	if(get_type() == BcacheType::FLASH_ONLY)
	    ST_THROW(LogicException("A Caching Set cannot be added to a flash-only bcache"));

	if(has_bcache_cset())
	    ST_THROW(LogicException("The bcache is already associated to a Caching Set"));

	User::create(get_devicegraph(), bcache_cset, get_non_impl());
    }


    void
    Bcache::Impl::remove_bcache_cset()
    {
	if(get_type() == BcacheType::FLASH_ONLY)
	    ST_THROW(LogicException("A Caching Set cannot be removed from a flash-only bcache"));

	if(!has_bcache_cset())
	    ST_THROW(LogicException("The bcache does not have an associated Caching Set"));

	User* user = to_user(get_devicegraph()->find_holder(get_bcache_cset()->get_sid(), get_sid()));

	get_devicegraph()->remove_holder(user);
    }


    void
    Bcache::Impl::update_sysfs_name_and_path()
    {
	set_sysfs_name(get_name().substr(strlen(DEV_DIR "/")));
	set_sysfs_path("/devices/virtual/block/" + get_sysfs_name());
    }



    bool
    Bcache::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
            return false;

        return type == rhs.type
	    && cache_mode == rhs.cache_mode
            && writeback_percent == rhs.writeback_percent
            && sequential_cutoff == rhs.sequential_cutoff;
    }


    void
    Bcache::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

        storage::log_diff(log, "type", toString(type), toString(rhs.type));
        storage::log_diff(log, "cache-mode", toString(cache_mode), toString(rhs.cache_mode));
        storage::log_diff(log, "writeback-percent", writeback_percent, rhs.writeback_percent);
        storage::log_diff(log, "sequential-cutoff", sequential_cutoff, rhs.sequential_cutoff);
    }


    void
    Bcache::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	out << " type:" << toString(type);

	if(get_type() == BcacheType::BACKED)
	{
	    out << " cache mode:" << toString(cache_mode)
		<< " writeback percent:" << writeback_percent << "%"
		<< " sequential cutoff:" << sequential_cutoff;
	}
    }


    void
    Bcache::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region();
    }


    void
    Bcache::Impl::calculate_region()
    {
	if(get_type() == BcacheType::BACKED)
	{
	    const BlkDevice* blk_device = get_backing_device();

	    unsigned long long size = blk_device->get_size();

	    if (size > metadata_size)
		size -= metadata_size;
	    else
		size = 0 * B;

	    set_size(size);
	}
    }


    void
    Bcache::Impl::run_dependency_manager(Actiongraph::Impl& actiongraph)
    {
	struct AllActions
	{
	    vector<Actiongraph::Impl::vertex_descriptor> delete_actions;
	    vector<Actiongraph::Impl::vertex_descriptor> create_actions;
	};

	// Build vectors with all actions.

	AllActions all_actions;

	for (Actiongraph::Impl::vertex_descriptor vertex : actiongraph.vertices())
	{
	    const Action::Base* action = actiongraph[vertex];

	    const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	    if (create_action)
	    {
		const Device* device = create_action->get_device(actiongraph);
		if (is_bcache(device))
		    all_actions.create_actions.push_back(vertex);
	    }

	    const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
	    if (delete_action)
	    {
		const Device* device = delete_action->get_device(actiongraph);
		if (is_bcache(device))
		    all_actions.delete_actions.push_back(vertex);
	    }
	}

	// Some functions used for sorting actions by bcache number.

	const Devicegraph* devicegraph_rhs = actiongraph.get_devicegraph(RHS);
	const Devicegraph* devicegraph_lhs = actiongraph.get_devicegraph(LHS);

	std::function<unsigned int(Actiongraph::Impl::vertex_descriptor)> key_fnc1 =
	    [&actiongraph, &devicegraph_lhs](Actiongraph::Impl::vertex_descriptor vertex) {
	    const Action::Base* action = actiongraph[vertex];
	    const Bcache* bcache = to_bcache(devicegraph_lhs->find_device(action->sid));
	    return bcache->get_number();
	};

	std::function<unsigned int(Actiongraph::Impl::vertex_descriptor)> key_fnc2 =
	    [&actiongraph, &devicegraph_rhs](Actiongraph::Impl::vertex_descriptor vertex) {
	    const Action::Base* action = actiongraph[vertex];
	    const Bcache* bcache = to_bcache(devicegraph_rhs->find_device(action->sid));
	    return bcache->get_number();
	};

	// Iterate over all bcaches with actions and build chain of
	// dependencies.

	vector<Actiongraph::Impl::vertex_descriptor> actions;

	all_actions.delete_actions = sort_by_key(all_actions.delete_actions, key_fnc1);
	actions.insert(actions.end(), all_actions.delete_actions.rbegin(),
		       all_actions.delete_actions.rend());

	all_actions.create_actions = sort_by_key(all_actions.create_actions, key_fnc2);
	actions.insert(actions.end(), all_actions.create_actions.begin(),
		       all_actions.create_actions.end());

	actiongraph.add_chain(actions);
    }


    void
    Bcache::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	// TODO Flash-only Bache

	if(get_type() == BcacheType::BACKED)
	{
	    vector<Action::Base*> actions;

	    actions.push_back(new Action::Create(get_sid()));

	    if (has_bcache_cset())
		actions.push_back(new Action::AttachBcacheCset(get_sid()));

	    actiongraph.add_chain(actions);
	}
    }


    void
    Bcache::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const
    {
	const Bcache* lhs_bcache = to_bcache(lhs);

	// TODO Flash-only Bcache

	if(get_type() == BcacheType::BACKED)
	{
	    vector<Action::Base*> actions;

	    if(lhs_bcache->has_bcache_cset() && has_bcache_cset())
	    {
		if(!lhs_bcache->get_bcache_cset()->get_impl().equal(get_bcache_cset()->get_impl()))
		{
		    actions.push_back(new Action::DetachBcacheCset(get_sid(), lhs_bcache->get_bcache_cset()));
		    actions.push_back(new Action::AttachBcacheCset(get_sid()));
		}
	    }
	    else if(lhs_bcache->has_bcache_cset() && !has_bcache_cset())
	    {
		actions.push_back(new Action::DetachBcacheCset(get_sid(), lhs_bcache->get_bcache_cset()));
	    }
	    else if(!lhs_bcache->has_bcache_cset() && has_bcache_cset())
	    {
		actions.push_back(new Action::AttachBcacheCset(get_sid()));
	    }

	    if(lhs_bcache->get_cache_mode() != get_cache_mode())
		actions.push_back(new Action::UpdateCacheMode(get_sid()));

	    actiongraph.add_chain(actions);
	}
    }


    void
    Bcache::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	// TODO Flash-only Bache

	if(get_type() == BcacheType::BACKED)
	{
	    vector<Action::Base*> actions;

	    if (is_active())
		actions.push_back(new Action::Deactivate(get_sid()));

	    actions.push_back(new Action::Delete(get_sid()));

	    actiongraph.add_chain(actions);
	}
    }


    Text
    Bcache::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Create bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Creating bcache %1$s (%2$s)"));

	return sformat(text, get_name(), get_size_text());
    }


    void
    Bcache::Impl::do_create()
    {
	// TODO Flash-only Bache

	if(get_type() == BcacheType::BACKED)
	{
	    const BlkDevice* backing_device = get_backing_device();

	    string cmd_line = BCACHE_BIN " make -B " + quote(backing_device->get_name());

	    wait_for_devices({ backing_device });

	    SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	    wait_for_devices({ get_non_impl() });

	    cmd_line = BCACHE_BIN " set-cachemode " + quote(backing_device->get_name()) + " " + quote(toString(cache_mode));

	    SystemCmd cmd2(cmd_line, SystemCmd::DoThrow);
	}
    }


    Text
    Bcache::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Delete bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Deleting bcache %1$s (%2$s)"));

	return sformat(text, get_name(), get_size_text());
    }


    void
    Bcache::Impl::do_delete() const
    {
	// TODO Flash-only Bache

	if(get_type() == BcacheType::BACKED)
	    get_backing_device()->get_impl().wipe_device();
    }


    Text
    Bcache::Impl::do_deactivate_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Deactivate bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Deactivating bcache %1$s (%2$s)"));

	return sformat(text, get_displayname(), get_size_text());
    }


    void
    Bcache::Impl::do_deactivate() const
    {
	string cmd_line = BCACHE_BIN " unregister " + quote(get_backing_device()->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	wait_for_detach_devices({ get_non_impl() });
    }


    Text
    Bcache::Impl::do_attach_bcache_cset_text(Tense tense) const
    {
	// TODO handle multiple BlkDevices of BcacheCset

	const BcacheCset* bcache_cset = get_bcache_cset();

	const BlkDevice* blk_device = bcache_cset->get_blk_devices()[0];

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by device name (e.g. /dev/bcache0),
			   // %3$s is replaced by size (e.g. 2 GiB)
			   _("Attach bcache cache set on %1$s to bcache %2$s (%3$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by device name (e.g. /dev/bcache0),
			   // %3$s is replaced by size (e.g. 2 GiB)
			   _("Attaching bcache cache set on %1$s to bcache %2$s (%3$s)"));

	return sformat(text, blk_device->get_name(), get_name(), get_size_text());
    }


    void
    Bcache::Impl::do_attach_bcache_cset() const
    {
	string cmd_line = BCACHE_BIN " attach " + quote(get_bcache_cset()->get_uuid()) + " " + quote(get_backing_device()->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    Bcache::Impl::do_detach_bcache_cset_text(Tense tense, const BcacheCset* bcache_cset) const
    {
	// TODO handle multiple BlkDevices of BcacheCset

	const BlkDevice* blk_device = bcache_cset->get_blk_devices()[0];

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by device name (e.g. /dev/bcache0),
			   // %3$s is replaced by size (e.g. 2 GiB)
			   _("Detach bcache cache set on %1$s from bcache %2$s (%3$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by device name (e.g. /dev/bcache0),
			   // %3$s is replaced by size (e.g. 2 GiB)
			   _("Detaching bcache cache set on %1$s from bcache %2$s (%3$s)"));

	return sformat(text, blk_device->get_name(), get_name(), get_size_text());
    }


    void
    Bcache::Impl::do_detach_bcache_cset() const
    {
	string cmd_line = BCACHE_BIN " detach " + quote(get_backing_device()->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

    Text
    Bcache::Impl::do_update_cache_mode_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by cache mode (e.g. writeback),
			   // %2$s is replaced by device name (e.g. /dev/bcache0),
			   // %3$s is replaced by size (e.g. 2 GiB)
			   _("Set cache mode to %1$s for bcache %2$s (%3$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by cache mode (e.g. writeback),
			   // %2$s is replaced by device name (e.g. /dev/bcache0),
			   // %3$s is replaced by size (e.g. 2 GiB)
			   _("Setting cache mode to %1$s for bcache %2$s (%3$s)"));

	return sformat(text, toString(get_cache_mode()), get_name(), get_size_text());
    }


    void
    Bcache::Impl::do_update_cache_mode() const
    {
	string cmd_line = BCACHE_BIN " set-cachemode " + quote(get_backing_device()->get_name()) + " " + quote(toString(get_cache_mode()));

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Bcache::Impl::add_dependencies(Actiongraph::Impl& actiongraph) const
    {
	vector<Actiongraph::Impl::vertex_descriptor> bcache_actions;
	vector<Actiongraph::Impl::vertex_descriptor> bcache_cset_actions;

	// Look for the relevant actions
	for (Actiongraph::Impl::vertex_descriptor vertex : actiongraph.vertices())
	{
	    const Action::Base* action = actiongraph[vertex];

	    if (action_is_my_attach(action, actiongraph))
		bcache_actions.push_back(vertex);
	    else if (action_is_my_bcache_cset_create(action, actiongraph))
		bcache_cset_actions.push_back(vertex);
	}

	if (!bcache_actions.empty() && !bcache_cset_actions.empty())
	    actiongraph.add_chain({ bcache_cset_actions, bcache_actions });
    }


    bool
    Bcache::Impl::action_is_my_attach(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	const Action::AttachBcacheCset* attach = dynamic_cast<const Action::AttachBcacheCset*>(action);
	return attach && attach->sid == get_sid();
    }


    bool
    Bcache::Impl::action_is_my_bcache_cset_create(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	if (!has_bcache_cset())
	    return false;

	const Action::Create* create = dynamic_cast<const Action::Create*>(action);

	if (!create)
	    return false;

	const Devicegraph* devicegraph = actiongraph.get_devicegraph(RHS);

	const Device* device = devicegraph->find_device(create->sid);

	if (!is_bcache_cset(device))
	    return false;

	return to_bcache_cset(device)->get_sid() == get_bcache_cset()->get_sid();
    }


    namespace Action
    {

	Text
	AttachBcacheCset::text(const CommitData& commit_data) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    return bcache->get_impl().do_attach_bcache_cset_text(commit_data.tense);
	}


	void
	AttachBcacheCset::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    bcache->get_impl().do_attach_bcache_cset();
	}


	Text
	DetachBcacheCset::text(const CommitData& commit_data) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    return bcache->get_impl().do_detach_bcache_cset_text(commit_data.tense, get_bcache_cset());
	}


	void
	DetachBcacheCset::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    bcache->get_impl().do_detach_bcache_cset();
	}


	Text
	UpdateCacheMode::text(const CommitData& commit_data) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    return bcache->get_impl().do_update_cache_mode_text(commit_data.tense);
	}


	void
	UpdateCacheMode::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    bcache->get_impl().do_update_cache_mode();
	}

    }

}
