/*
 * Copyright (c) [2017-2019] SUSE LLC
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


#include "storage/Devices/DmRaidImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Prober.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/UsedFeatures.h"
#include "storage/Holders/User.h"
#include "storage/Utils/StorageTypes.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<DmRaid>::classname = "DmRaid";


    DmRaid::Impl::Impl(const string& name)
	: Partitionable::Impl(name), rotational(false)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid DmRaid name"));

	set_dm_table_name(name.substr(strlen(DEV_MAPPER_DIR "/")));
    }


    DmRaid::Impl::Impl(const string& name, const Region& region)
	: Partitionable::Impl(name, region), rotational(false)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid DmRaid name"));

	set_dm_table_name(name.substr(strlen(DEV_MAPPER_DIR "/")));
    }


    DmRaid::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), rotational(false)
    {
	getChildValue(node, "rotational", rotational);
    }


    string
    DmRaid::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("DM RAID").translated;
    }


    void
    DmRaid::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Partitionable::Impl::check(check_callbacks);

	if (!is_valid_name(get_name()))
	    ST_THROW(Exception("invalid name"));
    }


    bool
    DmRaid::Impl::is_valid_name(const string& name)
    {
	return boost::starts_with(name, DEV_MAPPER_DIR "/");
    }


    bool
    DmRaid::Impl::activate_dm_raids(const ActivateCallbacks* activate_callbacks)
    {
	y2mil("activate_dm_raids");

	string cmd_line = DMRAID_BIN " --activate yes --no_partitions";

	SystemCmd cmd(cmd_line);

	if (cmd.retcode() == 0)
	    SystemCmd(UDEVADM_BIN_SETTLE);

	return cmd.retcode() == 0;
    }


    bool
    DmRaid::Impl::deactivate_dm_raids()
    {
	y2mil("deactivate_dm_raids");

	string cmd_line = DMRAID_BIN " --activate no";

	SystemCmd cmd(cmd_line);

	if (cmd.retcode() == 0)
	    return true;

	// Unfortunately from the return value of dmraid it is unclear whether
	// there are no raids or some raid could not be deactivated.

	SystemInfo system_info;
	for (const CmdDmsetupInfo::value_type& value : system_info.getCmdDmsetupInfo())
	{
	    if (value.second.subsystem == "DMRAID")
		return false;
	}

	return true;
    }


    void
    DmRaid::Impl::probe_dm_raids(Prober& prober)
    {
	const CmdDmraid& cmd_dm_raid = prober.get_system_info().getCmdDmraid();

	for (const string& dm_name : cmd_dm_raid.get_entries())
	{
	    DmRaid* dm_raid = DmRaid::create(prober.get_system(), DEV_MAPPER_DIR "/" + dm_name);
	    dm_raid->get_impl().probe_pass_1a(prober);
	}
    }


    void
    DmRaid::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	SystemInfo& system_info = prober.get_system_info();

	const File& rotational_file = get_sysfs_file(system_info, "queue/rotational");
	rotational = rotational_file.get<bool>();
    }


    void
    DmRaid::Impl::probe_pass_1b(Prober& prober)
    {
	const CmdDmraid& cmd_dmraid = prober.get_system_info().getCmdDmraid();

	const CmdDmraid::Entry& entry = cmd_dmraid.get_entry(get_dm_table_name());

	for (const string& device : entry.devices)
	{
	    BlkDevice* blk_device = BlkDevice::Impl::find_by_any_name(prober.get_system(), device,
								      prober.get_system_info());
	    User::create(prober.get_system(), blk_device, get_non_impl());
	}
    }


    uf_t
    DmRaid::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return UF_DMRAID | Partitionable::Impl::used_features(used_features_dependency_type);
    }


    ResizeInfo
    DmRaid::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    DmRaid::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValueIf(node, "rotational", rotational, rotational);
    }


    void
    DmRaid::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot create dm raid"));
    }


    void
    DmRaid::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot delete dm raid"));
    }


    bool
    DmRaid::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return rotational == rhs.rotational;
    }


    void
    DmRaid::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "rotational", rotational, rhs.rotational);
    }


    void
    DmRaid::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	if (rotational)
	    out << " rotational";
    }


    void
    DmRaid::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	// See doc/udev.md.

	erase_if(udev_ids, [](const string& udev_id) {
	    return !boost::starts_with(udev_id, "raid-");
	});
    }

}
