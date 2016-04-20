

#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    string
    get_pt_type_name(PtType pt_type)
    {
	return toString(pt_type);
    }


    PartitionSlot::PartitionSlot()
	: region(), nr(0), name(), primary_slot(false), primary_possible(false),
	  extended_slot(false), extended_possible(false), logical_slot(false),
	  logical_possible(false)
    {
    }


    PartitionTable::PartitionTable(Impl* impl)
	: Device(impl)
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
    PartitionTable::delete_partition(const string& name)
    {
	return get_impl().delete_partition(name);
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


    const Partitionable*
    PartitionTable::get_partitionable() const
    {
	return get_impl().get_partitionable();
    }


    vector<PartitionSlot>
    PartitionTable::get_unused_partition_slots(AlignPolicy align_policy) const
    {
	return get_impl().get_unused_partition_slots(align_policy);
    }


    Region
    PartitionTable::align(const Region& region, AlignPolicy align_policy) const
    {
	return get_impl().align(region, align_policy);
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

}
