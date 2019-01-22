/*
 * Copyright (c) [2019] SUSE LLC
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


#include "storage/Devices/FlashBcacheImpl.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    FlashBcache*
    FlashBcache::create(Devicegraph* devicegraph, const string& name)
    {
	FlashBcache* ret = new FlashBcache(new FlashBcache::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    FlashBcache*
    FlashBcache::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	FlashBcache* ret = new FlashBcache(new FlashBcache::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    FlashBcache::FlashBcache(Impl* impl)
	: Bcache(impl)
    {
    }


    FlashBcache*
    FlashBcache::clone() const
    {
	return new FlashBcache(get_impl().clone());
    }


    FlashBcache::Impl&
    FlashBcache::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const FlashBcache::Impl&
    FlashBcache::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<FlashBcache*>
    FlashBcache::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<FlashBcache>();
    }


    vector<const FlashBcache*>
    FlashBcache::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const FlashBcache>();
    }


    bool
    is_flash_bcache(const Device* device)
    {
	return is_device_of_type<const FlashBcache>(device);
    }


    FlashBcache*
    to_flash_bcache(Device* device)
    {
	return to_device_of_type<FlashBcache>(device);
    }


    const FlashBcache*
    to_flash_bcache(const Device* device)
    {
	return to_device_of_type<const FlashBcache>(device);
    }

}
