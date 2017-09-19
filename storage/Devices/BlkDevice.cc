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


#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/FreeInfo.h"
#include "storage/FindBy.h"


namespace storage
{

    using namespace std;


    BlkDevice::BlkDevice(Impl* impl)
	: Device(impl)
    {
    }


    BlkDevice::Impl&
    BlkDevice::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BlkDevice::Impl&
    BlkDevice::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    BlkDevice*
    BlkDevice::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return storage::find_by_name<BlkDevice>(devicegraph, name);
    }


    const BlkDevice*
    BlkDevice::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return storage::find_by_name<const BlkDevice>(devicegraph, name);
    }


    void
    BlkDevice::set_name(const string& name)
    {
	get_impl().set_name(name);
    }


    const string&
    BlkDevice::get_name() const
    {
	return get_impl().get_name();
    }


    const Region&
    BlkDevice::get_region() const
    {
	return get_impl().get_region();
    }


    void
    BlkDevice::set_region(const Region& region)
    {
	get_impl().set_region(region);
    }


    unsigned long long
    BlkDevice::get_size() const
    {
	return get_impl().get_size();
    }


    void
    BlkDevice::set_size(unsigned long long size)
    {
	get_impl().set_size(size);
    }


    string
    BlkDevice::get_size_string() const
    {
	return get_impl().get_size_string();
    }


    const string&
    BlkDevice::get_sysfs_name() const
    {
	return get_impl().get_sysfs_name();
    }


    const string&
    BlkDevice::get_sysfs_path() const
    {
	return get_impl().get_sysfs_path();
    }


    const vector<string>&
    BlkDevice::get_udev_paths() const
    {
	return get_impl().get_udev_paths();
    }


    const vector<string>&
    BlkDevice::get_udev_ids() const
    {
	return get_impl().get_udev_ids();
    }


    const string&
    BlkDevice::get_dm_table_name() const
    {
	return get_impl().get_dm_table_name();
    }


    void
    BlkDevice::set_dm_table_name(const string& dm_table_name)
    {
	get_impl().set_dm_table_name(dm_table_name);
    }


    vector<BlkDevice*>
    BlkDevice::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<BlkDevice>();
    }


    vector<const BlkDevice*>
    BlkDevice::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const BlkDevice>();
    }


    BlkFilesystem*
    BlkDevice::create_blk_filesystem(FsType fs_type)
    {
	return get_impl().create_blk_filesystem(fs_type);
    }


    bool
    BlkDevice::has_blk_filesystem() const
    {
	return get_impl().has_blk_filesystem();
    }


    BlkFilesystem*
    BlkDevice::get_blk_filesystem()
    {
	return get_impl().get_blk_filesystem();
    }


    const BlkFilesystem*
    BlkDevice::get_blk_filesystem() const
    {
	return get_impl().get_blk_filesystem();
    }


    Encryption*
    BlkDevice::create_encryption(const std::string& dm_name)
    {
	return get_impl().create_encryption(dm_name);
    }


    void
    BlkDevice::remove_encryption()
    {
	get_impl().remove_encryption();
    }


    bool
    BlkDevice::has_encryption() const
    {
	return get_impl().has_encryption();
    }


    Encryption*
    BlkDevice::get_encryption()
    {
	return get_impl().get_encryption();
    }


    const Encryption*
    BlkDevice::get_encryption() const
    {
	return get_impl().get_encryption();
    }


    bool
    is_blk_device(const Device* device)
    {
	return is_device_of_type<const BlkDevice>(device);
    }


    BlkDevice*
    to_blk_device(Device* device)
    {
	return to_device_of_type<BlkDevice>(device);
    }


    const BlkDevice*
    to_blk_device(const Device* device)
    {
	return to_device_of_type<const BlkDevice>(device);
    }

}
