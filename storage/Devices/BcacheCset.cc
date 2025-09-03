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


#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Devicegraph.h"
#include "storage/FindBy.h"


namespace storage
{

    using namespace std;


    BcacheCset*
    BcacheCset::create(Devicegraph* devicegraph)
    {
	shared_ptr<BcacheCset> bcache_cset = make_shared<BcacheCset>(make_unique<BcacheCset::Impl>());
	Device::Impl::create(devicegraph, bcache_cset);
	return bcache_cset.get();
    }


    BcacheCset*
    BcacheCset::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<BcacheCset> bcache_cset = make_shared<BcacheCset>(make_unique<BcacheCset::Impl>(node));
	Device::Impl::load(devicegraph, bcache_cset);
	return bcache_cset.get();
    }


    BcacheCset::BcacheCset(Impl* impl)
	: Device(impl)
    {
    }


    BcacheCset::BcacheCset(unique_ptr<Device::Impl>&& impl)
	: Device(std::move(impl))
    {
    }


    BcacheCset*
    BcacheCset::clone() const
    {
	return new BcacheCset(get_impl().clone());
    }


    std::unique_ptr<Device>
    BcacheCset::clone_v2() const
    {
	return make_unique<BcacheCset>(get_impl().clone());
    }


    BcacheCset::Impl&
    BcacheCset::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BcacheCset::Impl&
    BcacheCset::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    const string&
    BcacheCset::get_uuid() const
    {
	return get_impl().get_uuid();
    }


    void
    BcacheCset::set_uuid(const string& uuid)
    {
	get_impl().set_uuid(uuid);
    }


    vector<const BlkDevice*>
    BcacheCset::get_blk_devices() const
    {
	return get_impl().get_blk_devices();
    }


    vector<const Bcache*>
    BcacheCset::get_bcaches() const
    {
	return get_impl().get_bcaches();
    }


    vector<BcacheCset*>
    BcacheCset::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<BcacheCset>();
    }


    BcacheCset*
    BcacheCset::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<BcacheCset>(devicegraph, uuid);
    }


    const BcacheCset*
    BcacheCset::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const BcacheCset>(devicegraph, uuid);
    }


    vector<const BcacheCset*>
    BcacheCset::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const BcacheCset>();
    }


    bool
    BcacheCset::compare_by_uuid(const BcacheCset* lhs, const BcacheCset* rhs)
    {
	return lhs->get_uuid() < rhs->get_uuid();
    }


    bool
    is_bcache_cset(const Device* device)
    {
	return is_device_of_type<const BcacheCset>(device);
    }


    BcacheCset*
    to_bcache_cset(Device* device)
    {
	return to_device_of_type<BcacheCset>(device);
    }


    const BcacheCset*
    to_bcache_cset(const Device* device)
    {
	return to_device_of_type<const BcacheCset>(device);
    }

}
