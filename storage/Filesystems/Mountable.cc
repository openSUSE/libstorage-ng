/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Filesystems/MountableImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Filesystems/MountPoint.h"


namespace storage
{

    using namespace std;


    string
    get_fs_type_name(FsType fs_type)
    {
	return toString(fs_type);
    }


    string
    get_mount_by_name(MountByType mount_by_type)
    {
	return toString(mount_by_type);
    }


    bool
    Mountable::is_valid_path(FsType fs_type, const std::string& path)
    {
	switch (fs_type)
	{
	    case FsType::UNKNOWN:
	    case FsType::AUTO:
		return false;

	    case FsType::SWAP:
		return path == "swap" || path == "none";

	    default:
		return boost::starts_with(path, "/");
	}
    }


    Mountable::Mountable(Impl* impl)
	: Device(impl)
    {
    }


    Mountable::Mountable(unique_ptr<Device::Impl>&& impl)
	: Device(std::move(impl))
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


    bool
    Mountable::supports_mount() const
    {
	return get_impl().supports_mount();
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


    void
    Mountable::remove_mount_point()
    {
	get_impl().remove_mount_point();
    }


    bool
    Mountable::has_filesystem() const
    {
	return get_impl().has_filesystem();
    }


    Filesystem*
    Mountable::get_filesystem()
    {
	return get_impl().get_filesystem();
    }


    const Filesystem*
    Mountable::get_filesystem() const
    {
	return get_impl().get_filesystem();
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
