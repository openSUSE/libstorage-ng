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


#ifndef STORAGE_PARTITIONABLE_H
#define STORAGE_PARTITIONABLE_H


#include <vector>

#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTable.h"


namespace storage
{

    // abstract class

    class Partitionable : public BlkDevice
    {
    public:

	const Topology& get_topology() const; // deprecated since also available in BlkDevice
	void set_topology(const Topology& topology); // deprecated since also available in BlkDevice

	/**
	 * Query the range of device nodes available. That range is
	 * the number of block nodes in /dev that the partitionable
	 * and the partitions of the partitionable can have. So at
	 * most range - 1 partitions can be created.
	 */
	unsigned int get_range() const;

	/**
	 * Set the range of device nodes available.
	 *
	 * @note The library does not support to change the range in
	 * the system. The function is only for testsuites.
	 *
	 * @see get_range()
	 */
	void set_range(unsigned int range);

	/**
	 * Checks whether the partitionable is in general usable as a
	 * partitionable (can hold a partition table). This is not the case
	 * for some DASDs, see doc/dasd.md, and for host-managed zoned disks.
	 *
	 * Does not consider if the partitionable is already in use.
	 */
	bool is_usable_as_partitionable() const;

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

	/**
	 * Check whether the partitionable has a partition table.
	 */
	bool has_partition_table() const;

	/**
	 * Return the partition table of the partitionable.
	 *
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

	/**
	 * Get all Partitionables.
	 */
	static std::vector<Partitionable*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Partitionable*> get_all(const Devicegraph* devicegraph);

	static Partitionable* find_by_name(Devicegraph* devicegraph, const std::string& name);
	static const Partitionable* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Partitionable(Impl* impl);
	ST_NO_SWIG Partitionable(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Partitionable.
     *
     * @throw NullPointerException
     */
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
