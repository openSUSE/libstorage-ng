/*
 * Copyright (c) [2015-2023] SUSE LLC
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


#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    string
    get_pt_type_name(PtType pt_type)
    {
	return toString(pt_type);
    }


    PartitionSlot::PartitionSlot()
	: region(), number(0), name(), primary_slot(false), primary_possible(false),
	  extended_slot(false), extended_possible(false), logical_slot(false),
	  logical_possible(false)
    {
    }


    bool
    PartitionSlot::is_possible(PartitionType partition_type) const
    {
	switch (partition_type)
	{
	    case PartitionType::PRIMARY:
		return primary_possible;

	    case PartitionType::EXTENDED:
		return extended_possible;

	    case PartitionType::LOGICAL:
		return logical_possible;
	}

	ST_THROW(Exception("invalid partition type"));
    }


    std::ostream&
    operator<<(std::ostream& s, const PartitionSlot& partition_slot)
    {
	s << "region:" << partition_slot.region << " number:" << partition_slot.number
	  << " name:" << partition_slot.name;

	if (partition_slot.primary_slot)
	    s << " primary-slot";

	if (partition_slot.primary_possible)
	    s << " primary-possible";

	if (partition_slot.extended_slot)
	    s << " extended-slot";

	if (partition_slot.extended_possible)
	    s << " extended-possible";

	if (partition_slot.logical_slot)
	    s << " logical-slot";

	if (partition_slot.logical_possible)
	    s << " logical-possible";

	return s;
    }


    PartitionTable::PartitionTable(Impl* impl)
	: Device(impl)
    {
    }


    PartitionTable::PartitionTable(unique_ptr<Device::Impl>&& impl)
	: Device(std::move(impl))
    {
    }


    PartitionTable::Impl&
    PartitionTable::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const PartitionTable::Impl&
    PartitionTable::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    PtType
    PartitionTable::get_type() const
    {
	return get_impl().get_type();
    }


    Partition*
    PartitionTable::create_partition(const string& name, const Region& region, PartitionType type)
    {
	return get_impl().create_partition(name, region, type);
    }


    void
    PartitionTable::delete_partition(Partition* partition)
    {
	get_impl().delete_partition(partition);
    }


    void
    PartitionTable::delete_partition(const string& name)
    {
	get_impl().delete_partition(name);
    }


    unsigned int
    PartitionTable::max_primary() const
    {
	return get_impl().max_primary();
    }


    bool
    PartitionTable::extended_possible() const
    {
	return get_impl().extended_possible();
    }


    unsigned int
    PartitionTable::max_logical() const
    {
	return get_impl().max_logical();
    }


    unsigned int
    PartitionTable::num_primary() const
    {
	return get_impl().num_primary();
    }


    bool
    PartitionTable::has_extended() const
    {
	return get_impl().has_extended();
    }


    unsigned int
    PartitionTable::num_logical() const
    {
	return get_impl().num_logical();
    }


    vector<Partition*>
    PartitionTable::get_partitions()
    {
	return get_impl().get_partitions();
    }


    vector<const Partition*>
    PartitionTable::get_partitions() const
    {
	return get_impl().get_partitions();
    }


    Partition*
    PartitionTable::get_partition(const string& name)
    {
	return get_impl().get_partition(name);
    }


    const Partition*
    PartitionTable::get_extended() const
    {
	return get_impl().get_extended();
    }


    Partitionable*
    PartitionTable::get_partitionable()
    {
	return get_impl().get_partitionable();
    }


    const Partitionable*
    PartitionTable::get_partitionable() const
    {
	return get_impl().get_partitionable();
    }


    Alignment
    PartitionTable::get_alignment(AlignType align_type) const
    {
	return get_impl().get_alignment(align_type);
    }


    vector<PartitionSlot>
    PartitionTable::get_unused_partition_slots(AlignPolicy align_policy, AlignType align_type) const
    {
	return get_impl().get_unused_partition_slots(align_policy, align_type);
    }


    Region
    PartitionTable::align(const Region& region, AlignPolicy align_policy, AlignType align_type) const
    {
	return get_impl().align(region, align_policy, align_type);
    }


    bool
    is_partition_table(const Device* device)
    {
	return is_device_of_type<const PartitionTable>(device);
    }


    PartitionTable*
    to_partition_table(Device* device)
    {
	return to_device_of_type<PartitionTable>(device);
    }


    const PartitionTable*
    to_partition_table(const Device* device)
    {
	return to_device_of_type<const PartitionTable>(device);
    }


    bool
    PartitionTable::is_partition_boot_flag_supported() const
    {
	return get_impl().is_partition_boot_flag_supported();
    }


    bool
    PartitionTable::is_partition_legacy_boot_flag_supported() const
    {
	return get_impl().is_partition_legacy_boot_flag_supported();
    }


    bool
    PartitionTable::is_partition_no_automount_flag_supported() const
    {
	return get_impl().is_partition_no_automount_flag_supported();
    }


    bool
    PartitionTable::is_partition_id_supported(unsigned int id) const
    {
	return get_impl().is_partition_id_supported(id);
    }

}
