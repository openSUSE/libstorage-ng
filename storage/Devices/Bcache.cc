/*
 * Copyright (c) [2016-2023] SUSE LLC
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
    get_bcache_type_name(BcacheType bcache_type)
    {
	return toString(bcache_type);
    }


    string
    get_cache_mode_name(CacheMode cache_mode)
    {
	return toString(cache_mode);
    }


    Bcache*
    Bcache::create(Devicegraph* devicegraph, const string& name)
    {
	return Bcache::create(devicegraph, name, BcacheType::BACKED);
    }


    Bcache*
    Bcache::create(Devicegraph* devicegraph, const string& name, BcacheType type)
    {
	shared_ptr<Bcache> bcache = make_shared<Bcache>(make_unique<Bcache::Impl>(name, type));
	Device::Impl::create(devicegraph, bcache);
	return bcache.get();
    }


    Bcache*
    Bcache::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Bcache> bcache = make_shared<Bcache>(make_unique<Bcache::Impl>(node));
	Device::Impl::load(devicegraph, bcache);
	return bcache.get();
    }


    Bcache::Bcache(Impl* impl)
	: Partitionable(impl)
    {
    }


    Bcache::Bcache(unique_ptr<Device::Impl>&& impl)
	: Partitionable(std::move(impl))
    {
    }


    Bcache*
    Bcache::clone() const
    {
	return new Bcache(get_impl().clone());
    }


    std::unique_ptr<Device>
    Bcache::clone_v2() const
    {
	return make_unique<Bcache>(get_impl().clone());
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


    const BlkDevice*
    Bcache::get_backing_device() const
    {
	return get_impl().get_backing_device();
    }


    const BlkDevice*
    Bcache::get_blk_device() const
    {
	return get_backing_device();
    }


    bool
    Bcache::has_bcache_cset() const
    {
	return get_impl().has_bcache_cset();
    }


    const BcacheCset*
    Bcache::get_bcache_cset() const
    {
	return get_impl().get_bcache_cset();
    }


    void
    Bcache::attach_bcache_cset(BcacheCset* bcache_cset)
    {
	get_impl().attach_bcache_cset(bcache_cset);
    }


    void
    Bcache::add_bcache_cset(BcacheCset* bcache_cset)
    {
	get_impl().attach_bcache_cset(bcache_cset);
    }


    void
    Bcache::detach_bcache_cset()
    {
	get_impl().detach_bcache_cset();
    }


    void
    Bcache::remove_bcache_cset()
    {
	get_impl().detach_bcache_cset();
    }


    CacheMode
    Bcache::get_cache_mode() const
    {
	return get_impl().get_cache_mode();
    }


    void
    Bcache::set_cache_mode(CacheMode mode)
    {
	get_impl().set_cache_mode(mode);
    }


    unsigned long long
    Bcache::get_sequential_cutoff() const
    {
	return get_impl().get_sequential_cutoff();
    }


    unsigned
    Bcache::get_writeback_percent() const
    {
	return get_impl().get_writeback_percent();
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
