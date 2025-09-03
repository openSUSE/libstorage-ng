/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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


#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/Multipath.h"
#include "storage/Devices/DmRaid.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    Partitionable::Partitionable(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Partitionable::Partitionable(unique_ptr<Device::Impl>&& impl)
	: BlkDevice(std::move(impl))
    {
    }


    Partitionable::Impl&
    Partitionable::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Partitionable::Impl&
    Partitionable::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    const Topology&
    Partitionable::get_topology() const
    {
	return get_impl().get_topology();
    }


    void
    Partitionable::set_topology(const Topology& topology)
    {
	get_impl().set_topology(topology);
    }


    unsigned int
    Partitionable::get_range() const
    {
	return get_impl().get_range();
    }


    void
    Partitionable::set_range(unsigned int range)
    {
	return get_impl().set_range(range);
    }


    bool
    Partitionable::is_usable_as_partitionable() const
    {
	return get_impl().is_usable_as_partitionable();
    }


    PtType
    Partitionable::get_default_partition_table_type() const
    {
	return get_impl().get_default_partition_table_type();
    }


    std::vector<PtType>
    Partitionable::get_possible_partition_table_types() const
    {
	return get_impl().get_possible_partition_table_types();
    }


    PartitionTable*
    Partitionable::create_partition_table(PtType pt_type)
    {
	return get_impl().create_partition_table(pt_type);
    }


    bool
    Partitionable::has_partition_table() const
    {
	return get_impl().has_partition_table();
    }


    PartitionTable*
    Partitionable::get_partition_table()
    {
	return get_impl().get_partition_table();
    }


    const PartitionTable*
    Partitionable::get_partition_table() const
    {
	return get_impl().get_partition_table();
    }


    string
    Partitionable::partition_name(int number) const
    {
	return get_impl().partition_name(number);
    }


    std::vector<Partitionable*>
    Partitionable::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Partitionable>();
    }


    std::vector<const Partitionable*>
    Partitionable::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Partitionable>();
    }


    Partitionable*
    Partitionable::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_partitionable(BlkDevice::find_by_name(devicegraph, name));
    }


    const Partitionable*
    Partitionable::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_partitionable(BlkDevice::find_by_name(devicegraph, name));
    }


    bool
    is_partitionable(const Device* device)
    {
	return is_device_of_type<const Partitionable>(device);
    }


    Partitionable*
    to_partitionable(Device* device)
    {
	return to_device_of_type<Partitionable>(device);
    }


    const Partitionable*
    to_partitionable(const Device* device)
    {
	return to_device_of_type<const Partitionable>(device);
    }

}
