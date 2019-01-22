/*
 * Copyright (c) [2019] SUSE LLC
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

#include "storage/Devices/BackedBcacheImpl.h"
#include "storage/Utils/Algorithm.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Holders/User.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"


namespace storage
{

    using namespace std;


    // TODO synchronization after actions
    // TODO write sequential_cutoff and writeback_percent persistently to survive reboot

    const char* DeviceTraits<BackedBcache>::classname = "BackedBcache";

    const vector<string> EnumTraits<CacheMode>::names({
	"writethrough", "writeback", "writearound", "none"
    });

    BackedBcache::Impl::Impl(const string& name)
	: Bcache::Impl(name), cache_mode(CacheMode::WRITETHROUGH),
          sequential_cutoff(8 * MiB), writeback_percent(10) // bcache defaults
    {
    }


    BackedBcache::Impl::Impl(const xmlNode* node)
	: Bcache::Impl(node), cache_mode(CacheMode::WRITETHROUGH),
          sequential_cutoff(8 * MiB), writeback_percent(10) // bcache defaults
    {
	string tmp;

	cache_mode = CacheMode::WRITETHROUGH;
	if (getChildValue(node, "cache-mode", tmp))
	    cache_mode = toValueWithFallback(tmp, CacheMode::WRITETHROUGH);
	
	getChildValue(node, "writeback-percent", writeback_percent);
	getChildValue(node, "sequential-cutoff", sequential_cutoff);
    }


    string
    BackedBcache::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("BackedBcache").translated;
    }


    void
    BackedBcache::Impl::save(xmlNode* node) const
    {
	Bcache::Impl::save(node);

	setChildValue(node, "cache-mode", toString(cache_mode));
	setChildValue(node, "writeback-percent", writeback_percent);
	setChildValue(node, "sequential-cutoff", sequential_cutoff);
    }


    static CacheMode
    parse_mode(File cache_mode_file)
    {
	string modes = cache_mode_file.get<string>();
	regex rgx("\\[(.*)\\]");
	smatch match;
	if (regex_search(modes, match, rgx))
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

    static unsigned long long parse_size(File file)
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
    BackedBcache::Impl::probe_pass_1a(Prober& prober)
    {
	Bcache::Impl::probe_pass_1a(prober);

	SystemInfo& system_info = prober.get_system_info();

	const File cache_mode_file = system_info.getFile(SYSFS_DIR + get_sysfs_path() + "/bcache/cache_mode");
	set_cache_mode(parse_mode(cache_mode_file));

	const File writeback_percent_file = system_info.getFile(SYSFS_DIR + get_sysfs_path() + "/bcache/writeback_percent");
	set_writeback_percent(writeback_percent_file.get<unsigned>());

	const File sequential_cutoff_file = system_info.getFile(SYSFS_DIR + get_sysfs_path() + "/bcache/sequential_cutoff");
	set_sequential_cutoff(parse_size(sequential_cutoff_file));
    }


    void
    BackedBcache::Impl::probe_pass_1b(Prober& prober)
    {
	Bcache::Impl::probe_pass_1b(prober);

	// Creating relationship with its backing device
	const File dev_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() + "/bcache/../dev");
	string dev = DEV_DIR "/block/" + dev_file.get<string>();

	prober.add_holder(dev, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
	    User::create(system, a, b);
	});
    }


    const BlkDevice*
    BackedBcache::Impl::get_backing_device() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BlkDevice*> ret = devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));

	return ret.front();
    }


    bool
    BackedBcache::Impl::has_bcache_cset() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BcacheCset*> ret = devicegraph.filter_devices_of_type<const BcacheCset>(devicegraph.parents(vertex));

	return !ret.empty();
    }


    void
    BackedBcache::Impl::attach_bcache_cset(BcacheCset* bcache_cset)
    {
	// FIXME Does bcache support several csets? If not, raise exception if it already has one.
	User::create(get_devicegraph(), bcache_cset, get_non_impl());
    }


    bool
    BackedBcache::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Bcache::Impl::equal(rhs))
            return false;

        return cache_mode == rhs.cache_mode
            && writeback_percent == rhs.writeback_percent
            && sequential_cutoff == rhs.sequential_cutoff;
    }


    void
    BackedBcache::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Bcache::Impl::log_diff(log, rhs);

        storage::log_diff(log, "cache-mode", toString(cache_mode), toString(rhs.cache_mode));
        storage::log_diff(log, "writeback-percent", writeback_percent, rhs.writeback_percent);
        storage::log_diff(log, "sequential-cutoff", sequential_cutoff, rhs.sequential_cutoff);
    }


    void
    BackedBcache::Impl::print(std::ostream& out) const
    {
	Bcache::Impl::print(out);

        out << " cache mode:" << toString(cache_mode)
            << " writeback percent:" << writeback_percent << "%"
            << " sequential cutoff:" << sequential_cutoff;
    }


    void
    BackedBcache::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region();
    }


    void
    BackedBcache::Impl::calculate_region()
    {
	const BlkDevice* blk_device = get_backing_device();

	unsigned long long size = blk_device->get_size();

	if (size > metadata_size)
	    size -= metadata_size;
	else
	    size = 0 * B;

	set_size(size);
    }


    void
    BackedBcache::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));

	if (has_bcache_cset())
	    actions.push_back(new Action::AttachBcacheCset(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    BackedBcache::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	if (is_active())
	    actions.push_back(new Action::Deactivate(get_sid()));

	actions.push_back(new Action::Delete(get_sid()));

	actiongraph.add_chain(actions);
    }


    Text
    BackedBcache::Impl::do_create_text(Tense tense) const
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

	return sformat(text, get_name(), get_size_text());
    }


    void
    BackedBcache::Impl::do_create()
    {
	const BlkDevice* backing_device = get_backing_device();

	string cmd_line = MAKE_BCACHE_BIN " -B " + quote(backing_device->get_name());

	wait_for_devices({ backing_device });

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

        // TODO: is it enough? check with Coly how to recognize it
	sleep(1); // give bcache some time to finish its async operation, so bcache device exists

	// TODO: Action for edit attributes will be needed if edit of existing bcache is allowed
	cmd_line = ECHO_BIN " " + quote(toString(cache_mode)) + " > " + quote(SYSFS_DIR "/" + get_sysfs_path() + "/bcache/cache_mode");

	SystemCmd cmd2(cmd_line, SystemCmd::DoThrow);

	cmd_line = ECHO_BIN " " + to_string(writeback_percent) + " > " + quote(SYSFS_DIR "/" + get_sysfs_path() + "/bcache/writeback_percent");

	SystemCmd cmd3(cmd_line, SystemCmd::DoThrow);

	cmd_line = ECHO_BIN " " + to_string(sequential_cutoff) + " > " + quote(SYSFS_DIR "/" + get_sysfs_path() + "/bcache/sequential_cutoff");

	SystemCmd cmd4(cmd_line, SystemCmd::DoThrow);
    }


    Text
    BackedBcache::Impl::do_delete_text(Tense tense) const
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

	return sformat(text, get_name(), get_size_text());
    }


    void
    BackedBcache::Impl::do_delete() const
    {
	get_backing_device()->get_impl().wipe_device();
    }


    Text
    BackedBcache::Impl::do_deactivate_text(Tense tense) const
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

	return sformat(text, get_displayname(), get_size_text());
    }


    void
    BackedBcache::Impl::do_deactivate() const
    {
	string cmd_line = ECHO_BIN " 1 > " + quote(SYSFS_DIR "/" + get_sysfs_path() + "/bcache/stop");

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	sleep(1); // TODO
    }


    Text
    BackedBcache::Impl::do_attach_bcache_cset_text(Tense tense) const
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

	return sformat(text, blk_device->get_name(), get_name(), get_size_text());
    }


    void
    BackedBcache::Impl::do_attach_bcache_cset() const
    {
	const BcacheCset* bcache_cset = get_bcache_cset();

	string cmd_line = ECHO_BIN " " + quote(bcache_cset->get_uuid()) + " > " +
	    quote(SYSFS_DIR "/" + get_sysfs_path() + "/bcache/attach");

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	sleep(1); // TODO
    }


    namespace Action
    {

	Text
	AttachBcacheCset::text(const CommitData& commit_data) const
	{
	    const BackedBcache* bcache = to_backed_bcache(get_device(commit_data.actiongraph, RHS));
	    return bcache->get_impl().do_attach_bcache_cset_text(commit_data.tense);
	}


	void
	AttachBcacheCset::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const BackedBcache* bcache = to_backed_bcache(get_device(commit_data.actiongraph, RHS));
	    bcache->get_impl().do_attach_bcache_cset();
	}

    }

}
