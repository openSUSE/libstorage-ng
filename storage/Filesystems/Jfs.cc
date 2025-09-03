/*
 * Copyright (c) [2018-2023] SUSE LLC
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


#include "storage/Filesystems/JfsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Jfs*
    Jfs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Jfs> jfs = make_shared<Jfs>(make_unique<Jfs::Impl>());
	Device::Impl::create(devicegraph, jfs);
	return jfs.get();
    }


    Jfs*
    Jfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Jfs> jfs = make_shared<Jfs>(make_unique<Jfs::Impl>(node));
	Device::Impl::load(devicegraph, jfs);
	return jfs.get();
    }


    Jfs::Jfs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Jfs::Jfs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Jfs*
    Jfs::clone() const
    {
	return new Jfs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Jfs::clone_v2() const
    {
	return make_unique<Jfs>(get_impl().clone());
    }


    Jfs::Impl&
    Jfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Jfs::Impl&
    Jfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_jfs(const Device* device)
    {
	return is_device_of_type<const Jfs>(device);
    }


    Jfs*
    to_jfs(Device* device)
    {
	return to_device_of_type<Jfs>(device);
    }


    const Jfs*
    to_jfs(const Device* device)
    {
	return to_device_of_type<const Jfs>(device);
    }

}
