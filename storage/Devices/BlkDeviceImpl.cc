/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/FreeInfo.h"


namespace storage
{


    const char* DeviceTraits<BlkDevice>::classname = "BlkDevice";


    BlkDevice::Impl::Impl(const string& name)
	: Impl(name, Region(0, 0, 512))
    {
    }


    BlkDevice::Impl::Impl(const string& name, const Region& region)
	: Device::Impl(), name(name), region(region), dm_table_name()
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid BlkDevice name"));
    }


    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name(), region(0, 0, 512), dm_table_name()
    {
	if (!getChildValue(node, "name", name))
	    ST_THROW(Exception("no name"));

	getChildValue(node, "sysfs-name", sysfs_name);
	getChildValue(node, "sysfs-path", sysfs_path);

	getChildValue(node, "region", region);

	getChildValue(node, "udev-path", udev_path);
	getChildValue(node, "udev-id", udev_ids);

	getChildValue(node, "dm-table-name", dm_table_name);
    }


    void
    BlkDevice::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Device::Impl::probe_pass_1(probed, systeminfo);

	const CmdUdevadmInfo& cmdudevadminfo = systeminfo.getCmdUdevadmInfo(name);

	sysfs_name = cmdudevadminfo.get_name();
	sysfs_path = cmdudevadminfo.get_path();

	// region is probed in subclasses

	if (!cmdudevadminfo.get_by_path_links().empty())
	{
	    udev_path = cmdudevadminfo.get_by_path_links().front();
	    process_udev_path(udev_path);
	}

	if (!cmdudevadminfo.get_by_id_links().empty())
	{
	    udev_ids = cmdudevadminfo.get_by_id_links();
	    process_udev_ids(udev_ids);
	}
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);

	setChildValueIf(node, "sysfs-name", sysfs_name, !sysfs_name.empty());
	setChildValueIf(node, "sysfs-path", sysfs_path, !sysfs_path.empty());

	setChildValue(node, "region", region);

	setChildValueIf(node, "udev-path", udev_path, !udev_path.empty());
	setChildValueIf(node, "udev-id", udev_ids, !udev_ids.empty());

	setChildValueIf(node, "dm-table-name", dm_table_name, !dm_table_name.empty());
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

	// TODO inform children, e.g. Md
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
	// TODO handle all types of children

	const Filesystem* filesystem = get_filesystem();

	ResizeInfo resize_info = filesystem->get_impl().detect_resize_info();

	return resize_info;
    }


    BlkDevice*
    BlkDevice::Impl::find_by_name(Devicegraph* devicegraph, const string& name,
				  SystemInfo& systeminfo)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return blk_device;
	    }
	}

	dev_t majorminor = systeminfo.getCmdUdevadmInfo(name).get_majorminor();

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (systeminfo.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor() == majorminor)
		    return blk_device;
	    }
	}

	ST_THROW(DeviceNotFoundByName(name));
	__builtin_unreachable();
    }


    const BlkDevice*
    BlkDevice::Impl::find_by_name(const Devicegraph* devicegraph, const string& name,
				  SystemInfo& systeminfo)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return blk_device;
	    }
	}

	dev_t majorminor = systeminfo.getCmdUdevadmInfo(name).get_majorminor();

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (systeminfo.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor() == majorminor)
		    return blk_device;
	    }
	}

	ST_THROW(DeviceNotFoundByName(name));
	__builtin_unreachable();
    }


    void
    BlkDevice::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_size() != lhs.get_size())
	{
	    ResizeMode resize_mode = get_size() < lhs.get_size() ? ResizeMode::SHRINK :
		ResizeMode::GROW;

	    vector<Action::Base*> actions;

	    if (resize_mode == ResizeMode::GROW)
	    {
		actions.push_back(new Action::Resize(get_sid(), resize_mode));
	    }

	    // TODO handle children that cannot be resized, filesystems that
	    // are created must not be resized, encryption between partition
	    // and filesystem, ...

	    try
	    {
		const Filesystem* filesystem = get_filesystem();
		actions.push_back(new Action::Resize(filesystem->get_sid(), resize_mode));
	    }
	    catch (const Exception&)
	    {
	    }

	    if (resize_mode == ResizeMode::SHRINK)
	    {
		actions.push_back(new Action::Resize(get_sid(), resize_mode));
	    }

	    actiongraph.add_chain(actions);
	}
    }


    bool
    BlkDevice::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return name == rhs.name && sysfs_name == rhs.sysfs_name && sysfs_path == rhs.sysfs_path &&
	    region == rhs.region && dm_table_name == rhs.dm_table_name;
    }


    void
    BlkDevice::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);

	storage::log_diff(log, "sysfs-name", sysfs_name, rhs.sysfs_name);
	storage::log_diff(log, "sysfs-path", sysfs_path, rhs.sysfs_path);

	storage::log_diff(log, "region", region, rhs.region);

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

	out << " region:" << get_region();

	if (!udev_path.empty())
	    out << " udev-path:" << udev_path;

	if (!udev_ids.empty())
	    out << " udev-ids:" << udev_ids;

	if (!dm_table_name.empty())
	    out << " dm-table-name:" << dm_table_name;
    }


    bool
    BlkDevice::Impl::has_filesystem() const
    {
	return has_single_child_of_type<const Filesystem>();
    }


    Filesystem*
    BlkDevice::Impl::get_filesystem()
    {
	return get_single_child_of_type<Filesystem>();
    }


    const Filesystem*
    BlkDevice::Impl::get_filesystem() const
    {
	return get_single_child_of_type<const Filesystem>();
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
