/*
 * Copyright (c) [2015-2017] SUSE LLC
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


#ifndef STORAGE_PARTITION_TABLE_H
#define STORAGE_PARTITION_TABLE_H


#include <vector>

#include "storage/Utils/Swig.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/Alignment.h"
#include "storage/Devices/Device.h"
#include "storage/Devices/Partition.h"


namespace storage
{
    class Partitionable;


    //! Partition Table Type.
    enum class PtType {
	PT_UNKNOWN, PT_LOOP, MSDOS, GPT, DASD, MAC
    };

    std::string get_pt_type_name(PtType pt_type);


    class PartitionSlot
    {
    public:

	PartitionSlot();

	Region region;
	unsigned nr;
	std::string name;
	bool primary_slot;
	bool primary_possible;
	bool extended_slot;
	bool extended_possible;
	bool logical_slot;
	bool logical_possible;

	bool is_possible(PartitionType partition_type) const;
    };


    // abstract class

    class PartitionTable : public Device
    {
    public:

	PtType get_type() const;

	/**
	 * region is sector-based.
	 */
	Partition* create_partition(const std::string& name, const Region& region, PartitionType type);

	/**
	 * Delete a partition in the partition table. Also deletes all
	 * descendants of the partition.
	 */
	void delete_partition(Partition* partition);

	void delete_partition(const std::string& name) ST_DEPRECATED;

	unsigned int max_primary() const;
	bool extended_possible() const;
	unsigned int max_logical() const;

	unsigned int num_primary() const;
	bool has_extended() const;
	unsigned int num_logical() const;

	/**
	 * Sorted by partition number.
	 */
	std::vector<Partition*> get_partitions();

	/**
	 * @copydoc get_partitions()
	 */
	std::vector<const Partition*> get_partitions() const;

	Partition* get_partition(const std::string& name);

	const Partitionable* get_partitionable() const;

	Alignment get_alignment(AlignType align_type = AlignType::OPTIMAL) const;

	std::vector<PartitionSlot> get_unused_partition_slots(AlignPolicy align_policy = AlignPolicy::KEEP_END,
							      AlignType align_type = AlignType::OPTIMAL) const;

	/**
	 * region is sector-based.
	 */
	Region align(const Region& region, AlignPolicy align_policy = AlignPolicy::ALIGN_END,
		     AlignType align_type = AlignType::OPTIMAL) const;

	/**
	 * Returns whether the boot flag is supported on partitions on the
	 * partition table.
	 */
	bool is_partition_boot_flag_supported() const;

	/**
	 * Returns whether the legacy boot flag is supported on partitions on
	 * the partition table.
	 */
	bool is_partition_legacy_boot_flag_supported() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	PartitionTable(Impl* impl);

    };


    bool is_partition_table(const Device* device);

    PartitionTable* to_partition_table(Device* device);
    const PartitionTable* to_partition_table(const Device* device);

}

#endif
