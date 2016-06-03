/*
 * Copyright (c) 2016 SUSE LLC
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


#include <iostream>

#include "storage/Devices/LvmPvImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/FindBy.h"


namespace storage
{

    using namespace std;


    LvmPv*
    LvmPv::create(Devicegraph* devicegraph)
    {
	LvmPv* ret = new LvmPv(new LvmPv::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    LvmPv*
    LvmPv::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	LvmPv* ret = new LvmPv(new LvmPv::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    LvmPv::LvmPv(Impl* impl)
	: Device(impl)
    {
    }


    LvmPv*
    LvmPv::clone() const
    {
	return new LvmPv(get_impl().clone());
    }


    LvmPv::Impl&
    LvmPv::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const LvmPv::Impl&
    LvmPv::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    void
    LvmPv::check() const
    {
	Device::check();
    }


    vector<LvmPv*>
    LvmPv::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<LvmPv>();
    }


    vector<const LvmPv*>
    LvmPv::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const LvmPv>();
    }


    LvmPv*
    LvmPv::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<LvmPv>(devicegraph, uuid);
    }


    const LvmPv*
    LvmPv::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const LvmPv>(devicegraph, uuid);
    }


    bool
    LvmPv::has_blk_device() const
    {
	return get_impl().has_blk_device();
    }


    BlkDevice*
    LvmPv::get_blk_device()
    {
	return get_impl().get_blk_device();
    }


    const BlkDevice*
    LvmPv::get_blk_device() const
    {
	return get_impl().get_blk_device();
    }


    bool
    is_lvm_pv(const Device* device)
    {
	return is_device_of_type<const LvmPv>(device);
    }


    LvmPv*
    to_lvm_pv(Device* device)
    {
	return to_device_of_type<LvmPv>(device);
    }


    const LvmPv*
    to_lvm_pv(const Device* device)
    {
	return to_device_of_type<const LvmPv>(device);
    }

}
