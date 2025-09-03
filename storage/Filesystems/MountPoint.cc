/*
 * Copyright (c) [2017-2023] SUSE LLC
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
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    InvalidMountPointPath::InvalidMountPointPath(const string& path)
        : Exception(sformat("invalid path '%s' for mount point", path))
    {
    }


    MountPoint*
    MountPoint::create(Devicegraph* devicegraph, const string& path)
    {
	shared_ptr<MountPoint> mount_point = make_shared<MountPoint>(make_unique<MountPoint::Impl>(path));
	Device::Impl::create(devicegraph, mount_point);
	return mount_point.get();
    }


    MountPoint*
    MountPoint::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<MountPoint> mount_point = make_shared<MountPoint>(make_unique<MountPoint::Impl>(node));
	Device::Impl::load(devicegraph, mount_point);
	return mount_point.get();
    }


    MountPoint::MountPoint(Impl* impl)
	: Device(impl)
    {
    }


    MountPoint::MountPoint(unique_ptr<Device::Impl>&& impl)
	: Device(std::move(impl))
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


    bool
    MountPoint::is_rootprefixed() const
    {
	return get_impl().is_rootprefixed();
    }


    void
    MountPoint::set_rootprefixed(bool rootprefixed)
    {
	get_impl().set_rootprefixed(rootprefixed);
    }


    // Notes
    //
    // - boost::filesystem::path::canonical() is not what we want as this
    //   works with existing files (ours might not yet exist) and resolves
    //   symlinks (which is too restrictive)
    //
    // - boost::filesystem::path::lexically_normal() is also not what we
    //   want as it behaves a bit weird for our purpose; it changes e.g.
    //   '/foo/' into '/foo/.'
    //
    // - if you change this function keep in mind that 'swap' is a special
    //   valid path argument in libstorage-ng
    //
    string
    MountPoint::normalize_path(const string& path)
    {
        string tmp = regex_replace(path, regex("/+"), "/");

        if (tmp != "/")
        {
            tmp = regex_replace(tmp, regex("/$"), "");
        }

	return tmp;
    }


    MountByType
    MountPoint::get_mount_by() const
    {
	return get_impl().get_mount_by();
    }


    void
    MountPoint::set_mount_by(MountByType mount_by)
    {
	get_impl().set_mount_by(mount_by);
    }


    vector<MountByType>
    MountPoint::possible_mount_bys() const
    {
	return get_impl().possible_mount_bys();
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


    vector<string>
    MountPoint::default_mount_options() const
    {
	return get_impl().default_mount_options().get_opts();
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


    FsType
    MountPoint::get_mount_type() const
    {
	return get_impl().get_mount_type();
    }

    void
    MountPoint::set_mount_type(FsType mount_type)
    {
	get_impl().set_mount_type(mount_type);
    }

    int
    MountPoint::get_freq() const
    {
	return get_impl().get_freq();
    }


    void
    MountPoint::set_freq(int freq)
    {
	get_impl().set_freq(freq);
    }


    int
    MountPoint::get_passno() const
    {
	return get_impl().get_passno();
    }


    void
    MountPoint::set_passno(int passno)
    {
	get_impl().set_passno(passno);
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


    Filesystem*
    MountPoint::get_filesystem()
    {
	return get_impl().get_filesystem();
    }


    const Filesystem*
    MountPoint::get_filesystem() const
    {
	return get_impl().get_filesystem();
    }


    MountPoint*
    MountPoint::clone() const
    {
	return new MountPoint(get_impl().clone());
    }


    std::unique_ptr<Device>
    MountPoint::clone_v2() const
    {
	return make_unique<MountPoint>(get_impl().clone());
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
    MountPoint::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<MountPoint>();
    }


    vector<const MountPoint*>
    MountPoint::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const MountPoint>();
    }


    vector<const MountPoint*>
    MountPoint::find_by_path(const Devicegraph* devicegraph, const string& path)
    {
	auto pred = [&path](const MountPoint* mount_point) {
	    return mount_point->get_path() == path;
	};

	return devicegraph->get_impl().get_devices_of_type_if<const MountPoint>(pred);
    }


    void
    MountPoint::immediate_activate()
    {
	return get_impl().immediate_activate();
    }


    void
    MountPoint::immediate_deactivate()
    {
	return get_impl().immediate_deactivate();
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
