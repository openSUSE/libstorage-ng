/*
 * Copyright (c) 2020 Arvin Schnell
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


#include "storage/PoolImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    void
    Pool::Impl::add_device(const Device* device)
    {
	ST_CHECK_PTR(device);

	vector<sid_t>::const_iterator it = find(devices.begin(), devices.end(), device->get_sid());
	if (it != devices.end())
	    ST_THROW(Exception("device already in pool"));

	devices.push_back(device->get_sid());
    }


    void
    Pool::Impl::remove_device(const Device* device)
    {
	ST_CHECK_PTR(device);

	vector<sid_t>::const_iterator it = find(devices.begin(), devices.end(), device->get_sid());
	if (it == devices.end())
	    ST_THROW(Exception("device not in pool"));

	devices.erase(it);
    }


    vector<Device*>
    Pool::Impl::get_devices(Devicegraph* devicegraph) const
    {
	ST_CHECK_PTR(devicegraph);

	vector<Device*> ret;

	for (sid_t sid : devices)
	{
	    if (devicegraph->device_exists(sid))
		ret.push_back(devicegraph->find_device(sid));
	}

	return ret;
    }


    vector<const Device*>
    Pool::Impl::get_devices(const Devicegraph* devicegraph) const
    {
	ST_CHECK_PTR(devicegraph);

	vector<const Device*> ret;

	for (sid_t sid : devices)
	{
	    if (devicegraph->device_exists(sid))
		ret.push_back(devicegraph->find_device(sid));
	}

	return ret;
    }


    vector<Partitionable*>
    Pool::Impl::get_partitionable_candidates(Devicegraph* devicegraph) const
    {
	vector<Partitionable*> partitionables;

	for (Device* device : get_devices(devicegraph))
	{
	    if (!is_partitionable(device))
		continue;

	    Partitionable* partitionable = to_partitionable(device);
	    if (!partitionable->has_partition_table())
		continue;

	    partitionables.push_back(partitionable);
	}

	std::function<unsigned long long(Partitionable*)> key_fnc =
	    [](const Partitionable* partitionable) {
		const PartitionTable* partition_table = partitionable->get_partition_table();
		return partition_table->get_impl().get_used_size();
	    };

	return sort_by_key(partitionables, key_fnc);
    }


    unsigned long long
    Pool::Impl::max_partition_size(Devicegraph* devicegraph, unsigned int number) const
    {
	ST_CHECK_PTR(devicegraph);

	if (number == 0)
	    ST_THROW(Exception("illegal value of zero for number"));

	// For each partitionable find the biggest unused region.

	vector<unsigned long long> candidates;

	for (Partitionable* partitionable : get_partitionable_candidates(devicegraph))
	{
	    PartitionTable* partition_table = partitionable->get_partition_table();

	    vector<PartitionSlot> partition_slots = partition_table->get_unused_partition_slots();

	    vector<PartitionSlot>::const_iterator biggest = partition_slots.end();

	    for (vector<PartitionSlot>::const_iterator it = partition_slots.begin();
		 it != partition_slots.end(); ++it)
	    {
		if (!(it->primary_possible || it->logical_possible))
		    continue;

		if (biggest == partition_slots.end() || it->region > biggest->region)
		    biggest = it;
	    }

	    if (biggest == partition_slots.end())
		continue;

	    const Region& region = biggest->region;
	    candidates.push_back(region.to_bytes(region.get_length()));
	}

	if (candidates.size() < number)
	    ST_THROW(PoolOutOfSpace());

	// Find the smallest candidate among the 'number' biggest candidates.

	vector<unsigned long long>::iterator pos = candidates.begin() + number - 1;

	nth_element(candidates.begin(), pos, candidates.end(), std::greater<unsigned long long>());

	return *pos;
    }


    vector<Partition*>
    Pool::Impl::create_partitions(Devicegraph* devicegraph, unsigned int number, unsigned long long size) const
    {
	ST_CHECK_PTR(devicegraph);

	if (number == 0)
	    ST_THROW(Exception("illegal value of zero for number"));

	struct Candidate
	{
	    PartitionTable* partition_table;
	    string name;
	    Region region;
	};

	// For each partitionable find the smallest unused region that is still big
	// enough.

	vector<Candidate> candidates;

	for (Partitionable* partitionable : get_partitionable_candidates(devicegraph))
	{
	    PartitionTable* partition_table = partitionable->get_partition_table();

	    vector<PartitionSlot> partition_slots = partition_table->get_unused_partition_slots();

	    vector<PartitionSlot>::const_iterator best = partition_slots.end();

	    for (vector<PartitionSlot>::const_iterator it = partition_slots.begin();
		 it != partition_slots.end(); ++it)
	    {
		if (!(it->primary_possible || it->logical_possible))
		    continue;

		const Region& region = it->region;
		if (region.to_bytes(region.get_length()) < size)
		    continue;

		if (best == partition_slots.end() || it->region < best->region)
		    best = it;
	    }

	    if (best == partition_slots.end())
		continue;

	    Candidate candidate { partition_table, best->name, best->region };
	    candidates.push_back(candidate);

	    if (candidates.size() == number)
		break;
	}

	if (candidates.size() < number)
	    ST_THROW(PoolOutOfSpace());

	// TODO exceptions thrown below can result in only some partitions created

	vector<Partition*> partitions;

	for (const Candidate candidate : candidates)
	{
	    Region region = candidate.region;

	    region.set_length(size / region.get_block_size());
	    if (region.get_length() == 0)
		ST_THROW(Exception("requested size smaller than sector size"));

	    region = candidate.partition_table->align(region);

	    Partition* partition = candidate.partition_table->create_partition(candidate.name, region,
									       PartitionType::PRIMARY);

	    partitions.push_back(partition);
	}

	return partitions;
    }

}
