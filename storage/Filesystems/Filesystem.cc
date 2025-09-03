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


#include "storage/Utils/StorageTmpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Filesystem::Filesystem(Impl* impl)
	: Mountable(impl)
    {
    }


    Filesystem::Filesystem(unique_ptr<Device::Impl>&& impl)
	: Mountable(std::move(impl))
    {
    }


    Filesystem::Impl&
    Filesystem::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Filesystem::Impl&
    Filesystem::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<Filesystem*>
    Filesystem::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Filesystem>();
    }


    vector<const Filesystem*>
    Filesystem::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Filesystem>();
    }


    FsType
    Filesystem::get_type() const
    {
	return get_impl().get_type();
    }


    bool
    Filesystem::has_space_info() const
    {
	return get_impl().has_space_info();
    }


    SpaceInfo
    Filesystem::detect_space_info() const
    {
	return get_impl().detect_space_info();
    }


    void
    Filesystem::set_space_info(const SpaceInfo& space_info)
    {
	get_impl().set_space_info(space_info);
    }


    bool
    is_filesystem(const Device* device)
    {
	return is_device_of_type<const Filesystem>(device);
    }


    Filesystem*
    to_filesystem(Device* device)
    {
	return to_device_of_type<Filesystem>(device);
    }


    const Filesystem*
    to_filesystem(const Device* device)
    {
	return to_device_of_type<const Filesystem>(device);
    }

}
