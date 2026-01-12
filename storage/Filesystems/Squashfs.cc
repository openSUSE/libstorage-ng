/*
 * Copyright (c) [2017-2026] SUSE LLC
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


#include "storage/Filesystems/SquashfsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Squashfs*
    Squashfs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Squashfs> squashfs = make_shared<Squashfs>(make_unique<Squashfs::Impl>());
	Device::Impl::create(devicegraph, squashfs);
	return squashfs.get();
    }


    Squashfs*
    Squashfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Squashfs> squashfs = make_shared<Squashfs>(make_unique<Squashfs::Impl>(node));
	Device::Impl::load(devicegraph, squashfs);
	return squashfs.get();
    }


    Squashfs::Squashfs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Squashfs::Squashfs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Squashfs*
    Squashfs::clone() const
    {
	return new Squashfs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Squashfs::clone_v2() const
    {
	return make_unique<Squashfs>(get_impl().clone());
    }


    Squashfs::Impl&
    Squashfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Squashfs::Impl&
    Squashfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_squashfs(const Device* device)
    {
	return is_device_of_type<const Squashfs>(device);
    }


    Squashfs*
    to_squashfs(Device* device)
    {
	return to_device_of_type<Squashfs>(device);
    }


    const Squashfs*
    to_squashfs(const Device* device)
    {
	return to_device_of_type<const Squashfs>(device);
    }

}
