/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Filesystems/Ext4Impl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Ext4*
    Ext4::create(Devicegraph* devicegraph)
    {
	Ext4* ret = new Ext4(new Ext4::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Ext4*
    Ext4::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Ext4* ret = new Ext4(new Ext4::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Ext4::Ext4(Impl* impl)
	: Filesystem(impl)
    {
    }


    Ext4*
    Ext4::clone() const
    {
	return new Ext4(get_impl().clone());
    }


    Ext4::Impl&
    Ext4::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Ext4::Impl&
    Ext4::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_ext4(const Device* device)
    {
	return is_device_of_type<const Ext4>(device);
    }


    Ext4*
    to_ext4(Device* device)
    {
	return to_device_of_type<Ext4>(device);
    }


    const Ext4*
    to_ext4(const Device* device)
    {
	return to_device_of_type<const Ext4>(device);
    }

}
