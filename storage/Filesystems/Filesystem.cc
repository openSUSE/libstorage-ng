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


    string
    get_mount_by_name(MountByType mount_by_type)
    {
	return toString(mount_by_type);
    }


    Filesystem::Filesystem(Impl* impl)
	: Device(impl)
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


    const vector<string>&
    Filesystem::get_mountpoints() const
    {
	return get_impl().get_mountpoints();
    }


    void
    Filesystem::set_mountpoints(const vector<string>& mountpoints)
    {
	get_impl().set_mountpoints(mountpoints);
    }


    void
    Filesystem::add_mountpoint(const string& mountpoint)
    {
	return get_impl().add_mountpoint(mountpoint);
    }


    MountByType
    Filesystem::get_mount_by() const
    {
	return get_impl().get_mount_by();
    }


    void
    Filesystem::set_mount_by(MountByType mount_by)
    {
	get_impl().set_mount_by(mount_by);
    }


    const list<string>&
    Filesystem::get_fstab_options() const
    {
	return get_impl().get_fstab_options();
    }


    void
    Filesystem::set_fstab_options(const list<string>& fstab_options)
    {
	get_impl().set_fstab_options(fstab_options);
    }


    vector<Filesystem*>
    Filesystem::find_by_mountpoint(const Devicegraph* devicegraph, const string& mountpoint)
    {
	auto pred = [&mountpoint](const Filesystem* filesystem) {
	    return contains(filesystem->get_mountpoints(), mountpoint);
	};

	return devicegraph->get_impl().get_devices_of_type_if<Filesystem>(pred);
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
