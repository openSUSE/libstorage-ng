/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#include "storage/Filesystems/Ext2Impl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Ext2*
    Ext2::create(Devicegraph* devicegraph)
    {
	Ext2* ret = new Ext2(new Ext2::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Ext2*
    Ext2::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Ext2* ret = new Ext2(new Ext2::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Ext2::Ext2(Impl* impl)
	: Ext(impl)
    {
    }


    Ext2*
    Ext2::clone() const
    {
	return new Ext2(get_impl().clone());
    }


    Ext2::Impl&
    Ext2::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Ext2::Impl&
    Ext2::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_ext2(const Device* device)
    {
	return is_device_of_type<const Ext2>(device);
    }


    Ext2*
    to_ext2(Device* device)
    {
	return to_device_of_type<Ext2>(device);
    }


    const Ext2*
    to_ext2(const Device* device)
    {
	return to_device_of_type<const Ext2>(device);
    }

}
