#ifndef STORAGE_PARTITIONABLE_H
#define STORAGE_PARTITIONABLE_H


#include <vector>

#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Geometry.h"


namespace storage
{

    // abstract class

    class Partitionable : public BlkDevice
    {
    public:

	unsigned int get_range() const;
	void set_range(unsigned int range);

	const Geometry& get_geometry() const;
	void set_geometry(const Geometry& geometry);

	/**
	 * Get the default partition table type for the partitionable.
	 */
	PtType get_default_partition_table_type() const;

	/**
	 * Get possible partition table types for the disk. The first entry is
	 * identical to the default partition table type for the disk.
	 */
	std::vector<PtType> get_possible_partition_table_types() const;

	PartitionTable* create_partition_table(PtType pt_type);

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Partitionable(Impl* impl);

    };


    bool is_partitionable(const Device* device);

    Partitionable* to_partitionable(Device* device);
    const Partitionable* to_partitionable(const Device* device);

}

#endif
