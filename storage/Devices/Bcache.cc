/*
 * Copyright (c) [2016-2019] SUSE LLC
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


#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    string
    get_bcache_type_name(BcacheType type)
    {
	return toString(type);
    }


    Bcache::Bcache(Impl* impl)
	: Partitionable(impl)
    {
    }


    Bcache::Impl&
    Bcache::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Bcache::Impl&
    Bcache::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    BcacheType
    Bcache::get_type() const
    {
	return get_impl().get_type();
    }


    unsigned int
    Bcache::get_number() const
    {
	return get_impl().get_number();
    }


    const BcacheCset*
    Bcache::get_bcache_cset() const
    {
	return get_impl().get_bcache_cset();
    }


    vector<Bcache*>
    Bcache::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Bcache>();
    }


    vector<const Bcache*>
    Bcache::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Bcache>();
    }


    void
    Bcache::reassign_numbers(Devicegraph* devicegraph)
    {
	return Bcache::Impl::reassign_numbers(devicegraph);
    }


    Bcache*
    Bcache::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_bcache(BlkDevice::find_by_name(devicegraph, name));
    }


    const Bcache*
    Bcache::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_bcache(BlkDevice::find_by_name(devicegraph, name));
    }


    string
    Bcache::find_free_name(const Devicegraph* devicegraph)
    {
	return Bcache::Impl::find_free_name(devicegraph);
    }


    bool
    Bcache::compare_by_number(const Bcache* lhs, const Bcache* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }


    bool
    is_bcache(const Device* device)
    {
	return is_device_of_type<const Bcache>(device);
    }


    Bcache*
    to_bcache(Device* device)
    {
	return to_device_of_type<Bcache>(device);
    }


    const Bcache*
    to_bcache(const Device* device)
    {
	return to_device_of_type<const Bcache>(device);
    }

}
