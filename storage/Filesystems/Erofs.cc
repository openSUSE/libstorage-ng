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


#include "storage/Filesystems/ErofsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Erofs*
    Erofs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Erofs> erofs = make_shared<Erofs>(make_unique<Erofs::Impl>());
	Device::Impl::create(devicegraph, erofs);
	return erofs.get();
    }


    Erofs*
    Erofs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Erofs> erofs = make_shared<Erofs>(make_unique<Erofs::Impl>(node));
	Device::Impl::load(devicegraph, erofs);
	return erofs.get();
    }


    Erofs::Erofs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Erofs::Erofs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Erofs*
    Erofs::clone() const
    {
	return new Erofs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Erofs::clone_v2() const
    {
	return make_unique<Erofs>(get_impl().clone());
    }


    Erofs::Impl&
    Erofs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Erofs::Impl&
    Erofs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_erofs(const Device* device)
    {
	return is_device_of_type<const Erofs>(device);
    }


    Erofs*
    to_erofs(Device* device)
    {
	return to_device_of_type<Erofs>(device);
    }


    const Erofs*
    to_erofs(const Device* device)
    {
	return to_device_of_type<const Erofs>(device);
    }

}
