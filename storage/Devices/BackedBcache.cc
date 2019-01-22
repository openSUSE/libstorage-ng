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


#include "storage/Devices/BackedBcacheImpl.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    BackedBcache*
    BackedBcache::create(Devicegraph* devicegraph, const string& name)
    {
	BackedBcache* ret = new BackedBcache(new BackedBcache::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    BackedBcache*
    BackedBcache::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	BackedBcache* ret = new BackedBcache(new BackedBcache::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    BackedBcache::BackedBcache(Impl* impl)
	: Bcache(impl)
    {
    }


    BackedBcache*
    BackedBcache::clone() const
    {
	return new BackedBcache(get_impl().clone());
    }


    BackedBcache::Impl&
    BackedBcache::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BackedBcache::Impl&
    BackedBcache::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    const BlkDevice*
    BackedBcache::get_backing_device() const
    {
	return get_impl().get_backing_device();
    }


    bool
    BackedBcache::has_bcache_cset() const
    {
	return get_impl().has_bcache_cset();
    }


    void
    BackedBcache::attach_bcache_cset(BcacheCset* bcache_cset)
    {
	get_impl().attach_bcache_cset(bcache_cset);
    }


    CacheMode
    BackedBcache::get_cache_mode() const
    {
	return get_impl().get_cache_mode();
    }


    void
    BackedBcache::set_cache_mode(CacheMode mode)
    {
	get_impl().set_cache_mode(mode);
    }


    unsigned long long
    BackedBcache::get_sequential_cutoff() const
    {
	return get_impl().get_sequential_cutoff();
    }


    void
    BackedBcache::set_sequential_cutoff(unsigned long long size)
    {
	get_impl().set_sequential_cutoff(size);
    }


    unsigned
    BackedBcache::get_writeback_percent() const
    {
	return get_impl().get_writeback_percent();
    }


    void
    BackedBcache::set_writeback_percent(unsigned percent)
    {
	get_impl().set_writeback_percent(percent);
    }


    vector<BackedBcache*>
    BackedBcache::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<BackedBcache>();
    }


    vector<const BackedBcache*>
    BackedBcache::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const BackedBcache>();
    }


    bool
    is_backed_bcache(const Device* device)
    {
	return is_device_of_type<const BackedBcache>(device);
    }


    BackedBcache*
    to_backed_bcache(Device* device)
    {
	return to_device_of_type<BackedBcache>(device);
    }


    const BackedBcache*
    to_backed_bcache(const Device* device)
    {
	return to_device_of_type<const BackedBcache>(device);
    }

}
