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
#include "storage/Filesystems/MountableImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    string
    get_mount_by_name(MountByType mount_by_type)
    {
	return toString(mount_by_type);
    }


    Mountable::Mountable(Impl* impl)
	: Device(impl)
    {
    }


    Mountable::Impl&
    Mountable::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Mountable::Impl&
    Mountable::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<Mountable*>
    Mountable::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Mountable>();
    }


    vector<const Mountable*>
    Mountable::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Mountable>();
    }


    const vector<string>&
    Mountable::get_mountpoints() const
    {
	return get_impl().get_mountpoints();
    }


    void
    Mountable::set_mountpoints(const vector<string>& mountpoints)
    {
	get_impl().set_mountpoints(mountpoints);
    }


    void
    Mountable::add_mountpoint(const string& mountpoint)
    {
	return get_impl().add_mountpoint(mountpoint);
    }


    MountByType
    Mountable::get_mount_by() const
    {
	return get_impl().get_mount_by();
    }


    void
    Mountable::set_mount_by(MountByType mount_by)
    {
	get_impl().set_mount_by(mount_by);
    }


    const vector<string>&
    Mountable::get_mount_opts() const
    {
	return get_impl().get_mount_opts().get_opts();
    }


    void
    Mountable::set_mount_opts(const vector<string>& mount_opts)
    {
	get_impl().set_mount_opts(mount_opts);
    }


    vector<Mountable*>
    Mountable::find_by_mountpoint(const Devicegraph* devicegraph, const string& mountpoint)
    {
	auto pred = [&mountpoint](const Mountable* mountable) {
	    return contains(mountable->get_mountpoints(), mountpoint);
	};

	return devicegraph->get_impl().get_devices_of_type_if<Mountable>(pred);
    }


    bool
    is_mountable(const Device* device)
    {
	return is_device_of_type<const Mountable>(device);
    }


    Mountable*
    to_mountable(Device* device)
    {
	return to_device_of_type<Mountable>(device);
    }


    const Mountable*
    to_mountable(const Device* device)
    {
	return to_device_of_type<const Mountable>(device);
    }

}
