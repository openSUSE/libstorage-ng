/*
 * Copyright (c) 2017 SUSE LLC
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


#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    MountPoint*
    MountPoint::create(Devicegraph* devicegraph, const string& path)
    {
	MountPoint* ret = new MountPoint(new MountPoint::Impl(path));
	ret->Device::create(devicegraph);
	return ret;
    }


    MountPoint*
    MountPoint::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	MountPoint* ret = new MountPoint(new MountPoint::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    MountPoint::MountPoint(Impl* impl)
	: Device(impl)
    {
    }


    const string&
    MountPoint::get_path() const
    {
	return get_impl().get_path();
    }


    void
    MountPoint::set_path(const string& path)
    {
	get_impl().set_path(path);
    }


    MountByType
    MountPoint::get_mount_by() const
    {
	return get_impl().get_mount_by();
    }


    void
    MountPoint::set_mount_by(const MountByType mount_by)
    {
	get_impl().set_mount_by(mount_by);
    }


    void
    MountPoint::set_default_mount_by()
    {
	get_impl().set_default_mount_by();
    }


    void
    MountPoint::set_default_mount_options()
    {
	get_impl().set_default_mount_options();
    }


    const vector<string>&
    MountPoint::get_mount_options() const
    {
	return get_impl().get_mount_options().get_opts();
    }


    void
    MountPoint::set_mount_options(const vector<string>& mount_options)
    {
	get_impl().set_mount_options(mount_options);
    }


    int
    MountPoint::get_freq() const
    {
	return get_impl().get_freq();
    }


    int
    MountPoint::get_passno() const
    {
	return get_impl().get_passno();
    }


    bool
    MountPoint::is_active() const
    {
	return get_impl().is_active();
    }


    void
    MountPoint::set_active(bool active)
    {
	get_impl().set_active(active);
    }


    bool
    MountPoint::is_in_etc_fstab() const
    {
	return get_impl().is_in_etc_fstab();
    }


    void
    MountPoint::set_in_etc_fstab(bool in_etc_fstab)
    {
	get_impl().set_in_etc_fstab(in_etc_fstab);
    }


    bool
    MountPoint::has_mountable() const
    {
	return get_impl().has_mountable();
    }


    Mountable*
    MountPoint::get_mountable()
    {
	return get_impl().get_mountable();
    }


    const Mountable*
    MountPoint::get_mountable() const
    {
	return get_impl().get_mountable();
    }


    MountPoint*
    MountPoint::clone() const
    {
	return new MountPoint(get_impl().clone());
    }


    MountPoint::Impl&
    MountPoint::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const MountPoint::Impl&
    MountPoint::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<MountPoint*>
    MountPoint::find_by_path(const Devicegraph* devicegraph, const string& path)
    {
	auto pred = [&path](const MountPoint* mount_point) {
	    return mount_point->get_path() == path;
	};

	return devicegraph->get_impl().get_devices_of_type_if<MountPoint>(pred);
    }

    bool
    is_mount_point(const Device* device)
    {
	return is_device_of_type<const MountPoint>(device);
    }


    MountPoint*
    to_mount_point(Device* device)
    {
	return to_device_of_type<MountPoint>(device);
    }


    const MountPoint*
    to_mount_point(const Device* device)
    {
	return to_device_of_type<const MountPoint>(device);
    }

}
