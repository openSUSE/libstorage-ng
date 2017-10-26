/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#ifndef STORAGE_PARTITIONABLE_H
#define STORAGE_PARTITIONABLE_H


#include <vector>

#include "storage/Utils/Topology.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTable.h"


namespace storage
{

    // abstract class

    class Partitionable : public BlkDevice
    {
    public:

	const Topology& get_topology() const;
	void set_topology(const Topology& topology);

	unsigned int get_range() const;
	void set_range(unsigned int range);

	/**
	 * Get the default partition table type for the partitionable.
	 *
	 * @throw Exception
	 */
	PtType get_default_partition_table_type() const;

	/**
	 * Get possible partition table types for the disk. The first entry is
	 * identical to the default partition table type for the disk.
	 */
	std::vector<PtType> get_possible_partition_table_types() const;

	/**
	 * Create a partition table on the partitionable.
	 *
	 * When creating a implicit partition table this function also creates
	 * the implicit partition.
	 *
	 * @throw WrongNumberOfChildren, UnsupportedException, Exception
	 */
	PartitionTable* create_partition_table(PtType pt_type);

	bool has_partition_table() const;

	/**
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	PartitionTable* get_partition_table();

	/**
	 * Return the name of the partition with the specified number.
	 */
	virtual std::string partition_name(int number) const;

	/**
	 * @copydoc get_partition_table
	 */
	const PartitionTable* get_partition_table() const;

	static std::vector<Partitionable*> get_all(Devicegraph* devicegraph);
	static std::vector<const Partitionable*> get_all(const Devicegraph* devicegraph);

	static Partitionable* find_by_name(Devicegraph* devicegraph, const std::string& name);
	static const Partitionable* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Partitionable(Impl* impl);

    };


    bool is_partitionable(const Device* device);

    /**
     * Converts pointer to Device to pointer to Partitionable.
     *
     * @return Pointer to Partitionable.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Partitionable* to_partitionable(Device* device);

    /**
     * @copydoc to_partitionable(Device*)
     */
    const Partitionable* to_partitionable(const Device* device);

}

#endif
