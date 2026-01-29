/*
 * Copyright (c) [2016-2023] SUSE LLC
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

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Holders/User.h"
#include "storage/UsedFeatures.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Actions/DeactivateImpl.h"
#include "storage/Actions/DeleteImpl.h"


namespace storage
{

    using namespace std;


    // TODO bucket size?


    const char* DeviceTraits<BcacheCset>::classname = "BcacheCset";


    BcacheCset::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    string
    BcacheCset::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Bcache Cache Set").translated;
    }


    void
    BcacheCset::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    BcacheCset::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

	if (!get_uuid().empty() && !is_valid_uuid(get_uuid()))
	    ST_THROW(Exception("invalid uuid"));
    }


    uf_t
    BcacheCset::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return UF_BCACHE | Device::Impl::used_features(used_features_dependency_type);
    }


    ResizeInfo
    BcacheCset::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    bool
    BcacheCset::Impl::is_valid_uuid(const string& uuid)
    {
	static const regex uuid_regex(UUID_REGEX, regex::extended);

	return regex_match(uuid, uuid_regex);
    }


    void
    BcacheCset::Impl::probe_bcache_csets(Prober& prober)
    {
	for (const string& uuid : prober.get_system_info().getDir(SYSFS_DIR "/fs/bcache"))
	{
	    if (!is_valid_uuid(uuid))
		continue;

	    BcacheCset* bcache_cset = BcacheCset::create(prober.get_system());
	    bcache_cset->get_impl().set_uuid(uuid);
	}
    }


    void
    BcacheCset::Impl::probe_pass_1b(Prober& prober)
    {
	static const regex cache_regex("cache[0-9]+", regex::extended);
	static const regex bdev_regex("bdev[0-9]+", regex::extended);
	static const regex volume_regex("volume[0-9]+", regex::extended);

	Device::Impl::probe_pass_1b(prober);

	SystemInfo::Impl& system_info = prober.get_system_info();

	string path = SYSFS_DIR "/fs/bcache/" + uuid;

	const Dir& dir = system_info.getDir(path);
	for (const string& name : dir)
	{
	    if (regex_match(name, cache_regex))
	    {
		const File& dev_file = system_info.getFile(path + "/" + name + "/../dev");
		string dev = DEV_DIR "/block/" + dev_file.get<string>();

		prober.add_holder(dev, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
		    User::create(system, a, b);
		});
	    }

	    if (regex_match(name, bdev_regex))
	    {
		const File& dev_file = system_info.getFile(path + "/" + name + "/dev/dev");
		string dev = DEV_DIR "/block/" + dev_file.get<string>();

		prober.add_holder(dev, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
		    User::create(system, b, a);
		});
	    }

	    if (regex_match(name, volume_regex))
	    {
		const File& dev_file = system_info.getFile(path + "/" + name + "/../dev");
		string dev = DEV_DIR "/block/" + dev_file.get<string>();

		prober.add_holder(dev, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
		    User::create(system, b, a);
		});
	    }
	}
    }


    vector<const BlkDevice*>
    BcacheCset::Impl::get_blk_devices() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));
    }


    vector<const Bcache*>
    BcacheCset::Impl::get_bcaches() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<const Bcache>(devicegraph.children(vertex));
    }


    bool
    BcacheCset::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    BcacheCset::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    BcacheCset::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " uuid:" << uuid;
    }


    void
    BcacheCset::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Deactivate>(get_sid()));

	actions.push_back(make_shared<Action::Delete>(get_sid()));

	actiongraph.add_chain(actions);
    }


    Text
    BcacheCset::Impl::do_create_text(Tense tense) const
    {
	// TODO handle multiple BlkDevices

	const BlkDevice* blk_device = get_blk_devices()[0];

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Create bcache cache set on %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Creating bcache cache set %1$s (%2$s)"));

	return sformat(text, blk_device->get_name(), blk_device->get_impl().get_size_text());
    }


    void
    BcacheCset::Impl::do_create()
    {
	static const regex set_uuid_regex("Set UUID:[ \t]*([a-f0-9-]{36})", regex::extended);

	// TODO handle multiple BlkDevices?

	const BlkDevice* blk_device = get_blk_devices()[0];

	SystemCmd::Args cmd_args = { BCACHE_BIN, "make", "-C", blk_device->get_name() };

	wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);

	for (const string& line : cmd.stdout())
	{
	    smatch match;

	    if (regex_match(line, match, set_uuid_regex) && match.size() == 2)
	    {
		uuid = match[1];
		y2mil("found set-uuid " << uuid);
		break;
	    }
	}
    }


    Text
    BcacheCset::Impl::do_delete_text(Tense tense) const
    {
	// TODO handle multiple BlkDevices

	const BlkDevice* blk_device = get_blk_devices()[0];

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Delete bcache cache set on %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Deleting bcache cache set on %1$s (%2$s)"));

	return sformat(text, blk_device->get_name(), blk_device->get_impl().get_size_text());
    }


    void
    BcacheCset::Impl::do_delete() const
    {
        for (const BlkDevice* blk_device : get_blk_devices())
	    blk_device->get_impl().wipe_device();
    }


    Text
    BcacheCset::Impl::do_deactivate_text(Tense tense) const
    {
	// TODO handle multiple BlkDevices

	const BlkDevice* blk_device = get_blk_devices()[0];

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Deactivate bcache cache set on %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Deactivating bcache cache set on %1$s (%2$s)"));

	return sformat(text, blk_device->get_name(), blk_device->get_impl().get_size_text());
    }


    void
    BcacheCset::Impl::do_deactivate()
    {
	// TODO handle multiple caching devices?

	if (get_blk_devices().empty())
	    ST_THROW(DeviceNotFound("No caching devices"));

	const BlkDevice* blk_device = get_blk_devices().front();

	SystemCmd::Args cmd_args = { BCACHE_BIN, "unregister", blk_device->get_name() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);

	string cset_name = SYSFS_DIR "/fs/bcache/" + get_uuid();

	wait_for_detach_devices({ cset_name });
    }

}
