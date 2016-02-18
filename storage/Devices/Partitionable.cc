

#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    Partitionable::Partitionable(Impl* impl)
	: BlkDevice(impl)
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


    const Geometry&
    Partitionable::get_geometry() const
    {
	return get_impl().get_geometry();
    }


    void
    Partitionable::set_geometry(const Geometry& geometry)
    {
	get_impl().set_geometry(geometry);
    }


    unsigned int
    Partitionable::get_range() const
    {
	return get_impl().get_range();
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
