

#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    PartitionSlot::PartitionSlot()
	: region(), nr(0), name(), primary_slot(false), primary_possible(false),
	  extended_slot(false), extended_possible(false), logical_slot(false),
	  logical_possible(false)
    {
    }


    PartitionSlot::operator PartitionSlotInfo() const // legacy
    {
	PartitionSlotInfo info;

	info.cylRegion = region;
	info.nr = nr;
	info.device = name;
	info.primarySlot = primary_slot;
	info.primaryPossible = primary_possible;
	info.extendedSlot = extended_slot;
	info.extendedPossible = extended_possible;
	info.logicalSlot = logical_slot;
	info.logicalPossible = logical_possible;

	return info;
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


    const Disk*
    PartitionTable::get_disk() const
    {
	return get_impl().get_disk();
    }


    vector<PartitionSlot>
    PartitionTable::get_unused_partition_slots(bool all, bool logical) const
    {
	return get_impl().get_unused_partition_slots(all, logical);
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
