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
#include "storage/Filesystems/MountPoint.h"


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


    MountPoint*
    Mountable::create_mount_point(const string& path)
    {
	return get_impl().create_mount_point(path);
    }


    bool
    Mountable::has_mount_point() const
    {
	return get_impl().has_mount_point();
    }


    MountPoint*
    Mountable::get_mount_point()
    {
	return get_impl().get_mount_point();
    }


    const MountPoint*
    Mountable::get_mount_point() const
    {
	return get_impl().get_mount_point();
    }


    vector<string>
    Mountable::get_mountpoints() const
    {
	if (has_mount_point())
	{
	    const MountPoint* mount_point = get_mount_point();
	    return vector<string>({ mount_point->get_path() });
	}

	return vector<string>();
    }


    void
    Mountable::set_mountpoints(const vector<string>& mountpoints)
    {
	if (!mountpoints.empty())
	    add_mountpoint(mountpoints.front());
    }


    void
    Mountable::add_mountpoint(const string& mountpoint)
    {
	if (has_mount_point())
	    get_mount_point()->set_path(mountpoint);
	else
	    create_mount_point(mountpoint);
    }


    MountByType
    Mountable::get_mount_by() const
    {
	if (has_mount_point())
	{
	    const MountPoint* mount_point = get_mount_point();
	    return mount_point->get_mount_by();
	}

	return MountByType::DEVICE;
    }


    void
    Mountable::set_mount_by(MountByType mount_by)
    {
	MountPoint* mount_point = has_mount_point() ? get_mount_point() : create_mount_point("");
	mount_point->set_mount_by(mount_by);
    }


    const vector<string>&
    Mountable::get_mount_opts() const
    {
	if (has_mount_point())
	{
	    const MountPoint* mount_point = get_mount_point();
	    return mount_point->get_mount_options();
	}

	static const vector<string> empty;
	return empty;
    }


    void
    Mountable::set_mount_opts(const vector<string>& mount_opts)
    {
	MountPoint* mount_point = has_mount_point() ? get_mount_point() : create_mount_point("");
	mount_point->set_mount_options(mount_opts);
    }


    const vector<string>&
    Mountable::get_fstab_options() const
    {
	return get_mount_opts();
    }


    void
    Mountable::set_fstab_options(const vector<string>& mount_opts)
    {
	set_mount_opts(mount_opts);
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
