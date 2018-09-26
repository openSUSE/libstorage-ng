/*
 * Copyright (c) [2016-2018] SUSE LLC
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
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Holders/User.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Prober.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Storage.h"


namespace storage
{

    using namespace std;


    // TODO rename after delete (in memory)
    // TODO handle attach and detach
    // TODO synchronization after actions
    // TODO cache-mode


    const char* DeviceTraits<Bcache>::classname = "Bcache";


    Bcache::Impl::Impl(const string& name)
	: Partitionable::Impl(name)
    {
	update_sysfs_name_and_path();
    }


    Bcache::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node)
    {
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


    void
    Bcache::Impl::probe_bcaches(Prober& prober)
    {
	for (const string& short_name : prober.get_sys_block_entries().bcaches)
	{
	    string name = DEV_DIR "/" + short_name;

	    try
	    {
		Bcache* bcache = Bcache::create(prober.get_system(), name);
		bcache->get_impl().probe_pass_1a(prober);
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		error_callback(prober.get_probe_callbacks(), sformat(_("Probing bcache %s failed"),
								     name.c_str()), exception);
	    }
	}
    }


    void
    Bcache::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	const File size_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() + "/size");

	set_region(Region(0, size_file.get<unsigned long long>(), 512));
    }


    void
    Bcache::Impl::probe_pass_1b(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1b(prober);

	const File dev_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() + "/bcache/../dev");
	string dev = DEV_DIR "/block/" + dev_file.get<string>();

	prober.add_holder(dev, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
	    User::create(system, a, b);
	});
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


    const BlkDevice*
    Bcache::Impl::get_blk_device() const
    {
	// TODO, write some generic helper

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BlkDevice*> ret = devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));

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
    Bcache::Impl::attach_bcache_cset(BcacheCset* bcache_cset)
    {
	User::create(get_devicegraph(), bcache_cset, get_non_impl());
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

	return Partitionable::Impl::equal(rhs);
    }


    void
    Bcache::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);
    }


    void
    Bcache::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);
    }


    void
    Bcache::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region();
    }


    void
    Bcache::Impl::calculate_region()
    {
	const BlkDevice* blk_device = get_blk_device();

	unsigned long long size = blk_device->get_size();

	if (size > metadata_size)
	    size -= metadata_size;
	else
	    size = 0 * B;

	set_size(size);
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
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));

	if (has_bcache_cset())
	    actions.push_back(new Action::AttachBcacheCset(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Bcache::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	if (is_active())
	    actions.push_back(new Action::Deactivate(get_sid()));

	actions.push_back(new Action::Delete(get_sid()));

	actiongraph.add_chain(actions);
    }


    Text
    Bcache::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Create Bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Creating Bcache %1$s (%2$s)"));

	return sformat(text, get_name().c_str(), get_size_string().c_str());
    }


    void
    Bcache::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MAKE_BCACHE_BIN " -B " + quote(blk_device->get_name());

	wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	sleep(1);
    }


    Text
    Bcache::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Delete Bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Deleting Bcache %1$s (%2$s)"));

	return sformat(text, get_name().c_str(), get_size_string().c_str());
    }


    void
    Bcache::Impl::do_delete() const
    {
	get_blk_device()->get_impl().wipe_device();
    }


    Text
    Bcache::Impl::do_deactivate_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Deactivate Bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Deactivating Bcache %1$s (%2$s)"));

	return sformat(text, get_displayname().c_str(), get_size_string().c_str());
    }


    void
    Bcache::Impl::do_deactivate() const
    {
	string cmd_line = ECHO_BIN " 1 > " + quote(SYSFS_DIR "/" + get_sysfs_path() + "/bcache/stop");

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	sleep(1);		// TODO
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
			   _("Attach Bcache cache set on %1$s to Bcache %2$s (%3$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by device name (e.g. /dev/bcache0),
			   // %3$s is replaced by size (e.g. 2 GiB)
			   _("Attaching Bcache cache set on %1$s to Bcache %2$s (%3$s)"));

	return sformat(text, blk_device->get_name().c_str(), get_name().c_str(),
		       get_size_string().c_str());
    }


    void
    Bcache::Impl::do_attach_bcache_cset() const
    {
	const BcacheCset* bcache_cset = get_bcache_cset();

	string cmd_line = ECHO_BIN " " + quote(bcache_cset->get_uuid()) + " > " +
	    quote(SYSFS_DIR "/" + get_sysfs_path() + "/bcache/attach");

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	sleep(1);		// TODO
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

    }

}
