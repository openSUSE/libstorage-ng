/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) 2023 SUSE LLC
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


#include "storage/Filesystems/XfsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Xfs*
    Xfs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Xfs> xfs = make_shared<Xfs>(make_unique<Xfs::Impl>());
	Device::Impl::create(devicegraph, xfs);
	return xfs.get();
    }


    Xfs*
    Xfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Xfs> xfs = make_shared<Xfs>(make_unique<Xfs::Impl>(node));
	Device::Impl::load(devicegraph, xfs);
	return xfs.get();
    }


    Xfs::Xfs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Xfs::Xfs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Xfs*
    Xfs::clone() const
    {
	return new Xfs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Xfs::clone_v2() const
    {
	return make_unique<Xfs>(get_impl().clone());
    }


    Xfs::Impl&
    Xfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Xfs::Impl&
    Xfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_xfs(const Device* device)
    {
	return is_device_of_type<const Xfs>(device);
    }


    Xfs*
    to_xfs(Device* device)
    {
	return to_device_of_type<Xfs>(device);
    }


    const Xfs*
    to_xfs(const Device* device)
    {
	return to_device_of_type<const Xfs>(device);
    }

}
