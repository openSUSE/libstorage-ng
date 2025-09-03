/*
 * Copyright (c) [2017-2023] SUSE LLC
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


#include "storage/Devices/DmRaidImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    DmRaid*
    DmRaid::create(Devicegraph* devicegraph, const string& name)
    {
	shared_ptr<DmRaid> dm_raid = make_shared<DmRaid>(make_unique<DmRaid::Impl>(name));
	Device::Impl::create(devicegraph, dm_raid);
	return dm_raid.get();
    }


    DmRaid*
    DmRaid::create(Devicegraph* devicegraph, const string& name, const Region& region)
    {
	shared_ptr<DmRaid> dm_raid = make_shared<DmRaid>(make_unique<DmRaid::Impl>(name, region));
	Device::Impl::create(devicegraph, dm_raid);
	return dm_raid.get();
    }


    DmRaid*
    DmRaid::create(Devicegraph* devicegraph, const string& name, unsigned long long size)
    {
	shared_ptr<DmRaid> dm_raid = make_shared<DmRaid>(make_unique<DmRaid::Impl>(name, Region(0, size / 512, 512)));
	Device::Impl::create(devicegraph, dm_raid);
	return dm_raid.get();
    }


    DmRaid*
    DmRaid::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<DmRaid> dm_raid = make_shared<DmRaid>(make_unique<DmRaid::Impl>(node));
	Device::Impl::load(devicegraph, dm_raid);
	return dm_raid.get();
    }


    DmRaid::DmRaid(Impl* impl)
	: Partitionable(impl)
    {
    }


    DmRaid::DmRaid(unique_ptr<Device::Impl>&& impl)
	: Partitionable(std::move(impl))
    {
    }


    DmRaid*
    DmRaid::clone() const
    {
	return new DmRaid(get_impl().clone());
    }


    std::unique_ptr<Device>
    DmRaid::clone_v2() const
    {
	return make_unique<DmRaid>(get_impl().clone());
    }


    DmRaid::Impl&
    DmRaid::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const DmRaid::Impl&
    DmRaid::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    DmRaid*
    DmRaid::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_dm_raid(BlkDevice::find_by_name(devicegraph, name));
    }


    const DmRaid*
    DmRaid::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_dm_raid(BlkDevice::find_by_name(devicegraph, name));
    }


    vector<BlkDevice*>
    DmRaid::get_blk_devices()
    {
	return get_impl().get_blk_devices();
    }


    vector<const BlkDevice*>
    DmRaid::get_blk_devices() const
    {
	return get_impl().get_blk_devices();
    }


    bool
    DmRaid::is_rotational() const
    {
	return get_impl().is_rotational();
    }


    vector<DmRaid*>
    DmRaid::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<DmRaid>();
    }


    vector<const DmRaid*>
    DmRaid::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const DmRaid>();
    }


    bool
    is_dm_raid(const Device* device)
    {
	return is_device_of_type<const DmRaid>(device);
    }


    DmRaid*
    to_dm_raid(Device* device)
    {
	return to_device_of_type<DmRaid>(device);
    }


    const DmRaid*
    to_dm_raid(const Device* device)
    {
	return to_device_of_type<const DmRaid>(device);
    }

}
