/*
 * Copyright (c) 2015 Novell, Inc.
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
	Vfat* ret = new Vfat(new Vfat::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Vfat*
    Vfat::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Vfat* ret = new Vfat(new Vfat::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Vfat::Vfat(Impl* impl)
	: Filesystem(impl)
    {
    }


    Vfat*
    Vfat::clone() const
    {
	return new Vfat(get_impl().clone());
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
