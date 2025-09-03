/*
 * Copyright (c) [2015-2023] Novell, Inc.
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


#include "storage/Filesystems/VfatImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Vfat*
    Vfat::create(Devicegraph* devicegraph)
    {
	shared_ptr<Vfat> vfat = make_shared<Vfat>(make_unique<Vfat::Impl>());
	Device::Impl::create(devicegraph, vfat);
	return vfat.get();
    }


    Vfat*
    Vfat::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Vfat> vfat = make_shared<Vfat>(make_unique<Vfat::Impl>(node));
	Device::Impl::load(devicegraph, vfat);
	return vfat.get();
    }


    Vfat::Vfat(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Vfat::Vfat(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Vfat*
    Vfat::clone() const
    {
	return new Vfat(get_impl().clone());
    }


    std::unique_ptr<Device>
    Vfat::clone_v2() const
    {
	return make_unique<Vfat>(get_impl().clone());
    }


    Vfat::Impl&
    Vfat::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Vfat::Impl&
    Vfat::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_vfat(const Device* device)
    {
	return is_device_of_type<const Vfat>(device);
    }


    Vfat*
    to_vfat(Device* device)
    {
	return to_device_of_type<Vfat>(device);
    }


    const Vfat*
    to_vfat(const Device* device)
    {
	return to_device_of_type<const Vfat>(device);
    }

}
