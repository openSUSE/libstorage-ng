/*
 * Copyright (c) [2020-2023] SUSE LLC
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


#include "storage/Filesystems/TmpfsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Tmpfs*
    Tmpfs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Tmpfs> tmpfs = make_shared<Tmpfs>(make_unique<Tmpfs::Impl>());
	Device::Impl::create(devicegraph, tmpfs);
	return tmpfs.get();
    }


    Tmpfs*
    Tmpfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Tmpfs> tmpfs = make_shared<Tmpfs>(make_unique<Tmpfs::Impl>(node));
	Device::Impl::load(devicegraph, tmpfs);
	return tmpfs.get();
    }


    Tmpfs::Tmpfs(Impl* impl)
	: Filesystem(impl)
    {
    }


    Tmpfs::Tmpfs(unique_ptr<Device::Impl>&& impl)
	: Filesystem(std::move(impl))
    {
    }


    Tmpfs*
    Tmpfs::clone() const
    {
	return new Tmpfs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Tmpfs::clone_v2() const
    {
	return make_unique<Tmpfs>(get_impl().clone());
    }


    Tmpfs::Impl&
    Tmpfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Tmpfs::Impl&
    Tmpfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<Tmpfs*>
    Tmpfs::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Tmpfs>();
    }


    vector<const Tmpfs*>
    Tmpfs::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Tmpfs>();
    }


    bool
    is_tmpfs(const Device* device)
    {
	return is_device_of_type<const Tmpfs>(device);
    }


    Tmpfs*
    to_tmpfs(Device* device)
    {
	return to_device_of_type<Tmpfs>(device);
    }


    const Tmpfs*
    to_tmpfs(const Device* device)
    {
	return to_device_of_type<const Tmpfs>(device);
    }

}
