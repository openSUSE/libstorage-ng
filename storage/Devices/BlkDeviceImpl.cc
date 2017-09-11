/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#include <iostream>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Filesystems/Ext2.h"
#include "storage/Filesystems/Ext3.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/Reiserfs.h"
#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Filesystems/Ntfs.h"
#include "storage/Filesystems/Vfat.h"
#include "storage/Filesystems/Iso9660.h"
#include "storage/Filesystems/Udf.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/FreeInfo.h"
#include "storage/Prober.h"


namespace storage
{


    const char* DeviceTraits<BlkDevice>::classname = "BlkDevice";


    BlkDevice::Impl::Impl(const string& name)
	: Impl(name, Region(0, 0, 512))
    {
    }


    BlkDevice::Impl::Impl(const string& name, const Region& region)
	: Device::Impl(), name(name), active(true), region(region), udev_paths(), udev_ids(),
	  dm_table_name()
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid BlkDevice name"));
    }


    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name(), active(true), region(0, 0, 512), udev_paths(), udev_ids(),
	  dm_table_name()
    {
	if (!getChildValue(node, "name", name))
	    ST_THROW(Exception("no name"));

	getChildValue(node, "sysfs-name", sysfs_name);
	getChildValue(node, "sysfs-path", sysfs_path);

	getChildValue(node, "active", active);

	getChildValue(node, "region", region);

	getChildValue(node, "udev-path", udev_paths);
	getChildValue(node, "udev-id", udev_ids);

	getChildValue(node, "dm-table-name", dm_table_name);
    }


    void
    BlkDevice::Impl::probe_pass_1a(Prober& prober)
    {
	Device::Impl::probe_pass_1a(prober);

	if (active)
	{
	    const CmdUdevadmInfo& cmdudevadminfo = prober.get_system_info().getCmdUdevadmInfo(name);

	    sysfs_name = cmdudevadminfo.get_name();
	    sysfs_path = cmdudevadminfo.get_path();

	    // region is probed in subclasses

	    if (!cmdudevadminfo.get_by_path_links().empty())
	    {
		udev_paths = cmdudevadminfo.get_by_path_links();
		process_udev_paths(udev_paths);
	    }

	    if (!cmdudevadminfo.get_by_id_links().empty())
	    {
		udev_ids = cmdudevadminfo.get_by_id_links();
		process_udev_ids(udev_ids);
	    }
	}
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);

	setChildValueIf(node, "sysfs-name", sysfs_name, !sysfs_name.empty());
	setChildValueIf(node, "sysfs-path", sysfs_path, !sysfs_path.empty());

	setChildValueIf(node, "active", active, !active);

	setChildValue(node, "region", region);

	setChildValueIf(node, "udev-path", udev_paths, !udev_paths.empty());
	setChildValueIf(node, "udev-id", udev_ids, !udev_ids.empty());

	setChildValueIf(node, "dm-table-name", dm_table_name, !dm_table_name.empty());
    }


    void
    BlkDevice::Impl::check() const
    {
	Device::Impl::check();

	if (region.get_block_size() == 0)
	    ST_THROW(Exception(sformat("block size is zero for %s", get_name().c_str())));

	if (!is_valid_name(get_name()))
	    ST_THROW(Exception("BlkDevice has invalid name"));
    }


    void
    BlkDevice::Impl::set_name(const string& name)
    {
	Impl::name = name;
    }


    void
    BlkDevice::Impl::set_region(const Region& region)
    {
	Impl::region = region;

	for (Device* child : get_non_impl()->get_children())
	    child->get_impl().parent_has_new_region(get_non_impl());
    }


    unsigned long long
    BlkDevice::Impl::get_size() const
    {
	return region.to_bytes(region.get_length());
    }


    void
    BlkDevice::Impl::set_size(unsigned long long size)
    {
	// Direct to virtual set_region so that derived classes can perform
	// checks and that children can be informed.

	set_region(Region(region.get_start(), region.to_blocks(size), region.get_block_size()));
    }


    string
    BlkDevice::Impl::get_size_string() const
    {
	return byte_to_humanstring(get_size(), false, 2, false);
    }


    ResizeInfo
    BlkDevice::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info(true);

	for (const Device* child : get_non_impl()->get_children())
	    resize_info.combine(child->detect_resize_info());

	return resize_info;
    }


    bool
    BlkDevice::Impl::exists_by_name(const Devicegraph* devicegraph, const string& name,
				    SystemInfo& system_info)
    {
	if (!devicegraph->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong devicegraph"));

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return true;
	    }
	}

	dev_t majorminor = system_info.getCmdUdevadmInfo(name).get_majorminor();

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device && blk_device->get_impl().active)
	    {
		if (system_info.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor() == majorminor)
		    return true;
	    }
	}

	return false;
    }


    BlkDevice*
    BlkDevice::Impl::find_by_name(Devicegraph* devicegraph, const string& name,
				  SystemInfo& system_info)
    {
	if (!devicegraph->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong devicegraph"));

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return blk_device;
	    }
	}

	dev_t majorminor = system_info.getCmdUdevadmInfo(name).get_majorminor();

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device && blk_device->get_impl().active)
	    {
		if (system_info.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor() == majorminor)
		    return blk_device;
	    }
	}

	ST_THROW(DeviceNotFoundByName(name));
    }


    const BlkDevice*
    BlkDevice::Impl::find_by_name(const Devicegraph* devicegraph, const string& name,
				  SystemInfo& system_info)
    {
	if (!devicegraph->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong devicegraph"));

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return blk_device;
	    }
	}

	dev_t majorminor = system_info.getCmdUdevadmInfo(name).get_majorminor();

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (system_info.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor() == majorminor)
		    return blk_device;
	    }
	}

	ST_THROW(DeviceNotFoundByName(name));
    }


    void
    BlkDevice::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (lhs.get_size() != get_size())
	{
	    ResizeMode resize_mode = get_size() < lhs.get_size() ? ResizeMode::SHRINK :
		ResizeMode::GROW;

	    actiongraph.add_vertex(new Action::Resize(get_sid(), resize_mode));

	    for (const Device* child : get_non_impl()->get_children())
	    {
		// Only add a resize action for children that do not detect
		// their resize themself and that are new on RHS.

		if (is_filesystem(child) || is_lvm_pv(child))
		    if (actiongraph.get_devicegraph(LHS)->device_exists(child->get_sid()))
			actiongraph.add_vertex(new Action::Resize(child->get_sid(), resize_mode));
	    }
	}

	if (!lhs.is_active() && is_active())
	{
	    Action::Base* action = new Action::Activate(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.is_active() && !is_active())
	{
	    Action::Base* action = new Action::Deactivate(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    bool
    BlkDevice::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return name == rhs.name && sysfs_name == rhs.sysfs_name && sysfs_path == rhs.sysfs_path &&
	    region == rhs.region && active == rhs.active && udev_paths == rhs.udev_paths &&
	    udev_ids == rhs.udev_ids && dm_table_name == rhs.dm_table_name;
    }


    void
    BlkDevice::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);

	storage::log_diff(log, "sysfs-name", sysfs_name, rhs.sysfs_name);
	storage::log_diff(log, "sysfs-path", sysfs_path, rhs.sysfs_path);

	storage::log_diff(log, "active", active, rhs.active);

	storage::log_diff(log, "region", region, rhs.region);

	storage::log_diff(log, "udev-paths", udev_paths, rhs.udev_paths);
	storage::log_diff(log, "udev-ids", udev_ids, rhs.udev_ids);

	storage::log_diff(log, "dm-table-name", dm_table_name, rhs.dm_table_name);
    }


    void
    BlkDevice::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " name:" << get_name();

	if (!sysfs_name.empty())
	    out << " sysfs-name:" << sysfs_name;

	if (!sysfs_path.empty())
	    out << " sysfs-path:" << sysfs_path;

	if (!active)
	    out << " active:" << active;

	out << " region:" << get_region();

	if (!udev_paths.empty())
	    out << " udev-paths:" << udev_paths;

	if (!udev_ids.empty())
	    out << " udev-ids:" << udev_ids;

	if (!dm_table_name.empty())
	    out << " dm-table-name:" << dm_table_name;
    }


    typedef std::function<BlkFilesystem* (Devicegraph* devicegraph)> blk_filesystem_create_fnc;

    const map<FsType, blk_filesystem_create_fnc> blk_filesystem_create_registry = {
	{ FsType::BTRFS, &Btrfs::create },
	{ FsType::EXT2, &Ext2::create },
	{ FsType::EXT3, &Ext3::create },
	{ FsType::EXT4, &Ext4::create },
	{ FsType::ISO9660, &Iso9660::create },
	{ FsType::NTFS, &Ntfs::create },
	{ FsType::REISERFS, &Reiserfs::create },
	{ FsType::SWAP, &Swap::create },
	{ FsType::UDF, &Udf::create },
	{ FsType::VFAT, &Vfat::create },
	{ FsType::XFS, &Xfs::create }
    };


    BlkFilesystem*
    BlkDevice::Impl::create_blk_filesystem(FsType fs_type)
    {
	if (num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(num_children(), 0));

	map<FsType, blk_filesystem_create_fnc>::const_iterator it = blk_filesystem_create_registry.find(fs_type);
	if (it == blk_filesystem_create_registry.end())
	{
	    if (fs_type == FsType::NFS || fs_type == FsType::NFS4)
		ST_THROW(UnsupportedException("cannot create Nfs on BlkDevice"));

	    ST_THROW(UnsupportedException("unsupported filesystem type " + toString(fs_type)));
	}

	Devicegraph* devicegraph = get_devicegraph();

	BlkFilesystem* blk_filesystem = it->second(devicegraph);

	FilesystemUser::create(devicegraph, get_non_impl(), blk_filesystem);

	return blk_filesystem;
    }


    bool
    BlkDevice::Impl::has_blk_filesystem() const
    {
	return has_single_child_of_type<const BlkFilesystem>();
    }


    BlkFilesystem*
    BlkDevice::Impl::get_blk_filesystem()
    {
	return get_single_child_of_type<BlkFilesystem>();
    }


    const BlkFilesystem*
    BlkDevice::Impl::get_blk_filesystem() const
    {
	return get_single_child_of_type<const BlkFilesystem>();
    }


    Encryption*
    BlkDevice::Impl::create_encryption(const std::string& dm_name)
    {
	if (num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(num_children(), 0));

	Luks* luks = Luks::create(get_devicegraph(), dm_name);

	User::create(get_devicegraph(), get_non_impl(), luks);

	// TODO maybe add parent_added() next to parent_has_new_region() for this?
	luks->get_impl().parent_has_new_region(get_non_impl());

	return luks;
    }


    bool
    BlkDevice::Impl::has_encryption() const
    {
	return has_single_child_of_type<const Encryption>();
    }


    Encryption*
    BlkDevice::Impl::get_encryption()
    {
	return get_single_child_of_type<Encryption>();
    }


    const Encryption*
    BlkDevice::Impl::get_encryption() const
    {
	return get_single_child_of_type<const Encryption>();
    }


    void
    BlkDevice::Impl::wait_for_device() const
    {
	SystemCmd(UDEVADMBIN_SETTLE);

	bool exist = access(name.c_str(), R_OK) == 0;
	y2mil("name:" << name << " exist:" << exist);

	if (!exist)
	{
	    for (int count = 0; count < 500; ++count)
	    {
		usleep(10000);
		exist = access(name.c_str(), R_OK) == 0;
		if (exist)
		    break;
	    }
	    y2mil("name:" << name << " exist:" << exist);
	}

	if (!exist)
	    ST_THROW(Exception("wait_for_device failed"));
    }


    void
    BlkDevice::Impl::wipe_device() const
    {
	string cmd_line = WIPEFSBIN " --all " + quote(get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("wipefs failed"));
    }


    bool
    BlkDevice::Impl::is_valid_name(const string& name)
    {
	return boost::starts_with(name, DEVDIR "/");
    }


    bool
    compare_by_dm_table_name(const BlkDevice* lhs, const BlkDevice* rhs)
    {
	return lhs->get_dm_table_name() < rhs->get_dm_table_name();
    }


}
