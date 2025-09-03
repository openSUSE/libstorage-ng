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
	UNKNOWN, LOOP, MSDOS, GPT, DASD, MAC, IMPLICIT, AIX, SUN, ATARI, BSD, AMIGA, DVH
    };


    /**
     * Convert the PtType pt_type to a string.
     *
     * @see PtType
     */
    std::string get_pt_type_name(PtType pt_type);


    /**
     * Number and name are only valid if one of the possible flags is true.
     */
    class PartitionSlot
    {
    public:

	PartitionSlot();

	Region region;

	unsigned int number;
	unsigned nr() const ST_DEPRECATED { return number; }

	std::string name;

	bool primary_slot;
	bool primary_possible;
	bool extended_slot;
	bool extended_possible;
	bool logical_slot;
	bool logical_possible;

	bool is_possible(PartitionType partition_type) const;

	friend std::ostream& operator<<(std::ostream& s, const PartitionSlot& partition_slot);

    };


    // abstract class

    class PartitionTable : public Device
    {
    public:

	/**
	 * Get the partition type.
	 *
	 * @see PtType
	 */
	PtType get_type() const;

	/**
	 * region is sector-based.
	 *
	 * @throw DifferentBlockSizes
	 */
	Partition* create_partition(const std::string& name, const Region& region, PartitionType type);

	/**
	 * Delete a partition in the partition table. Also deletes all
	 * descendants of the partition.
	 */
	void delete_partition(Partition* partition);

	void delete_partition(const std::string& name) ST_DEPRECATED;

	/**
	 * Highest number for a primary or extended partition. Returns 0 if no
	 * primary or extended partition is possible.
	 *
	 * Lowest number for a primary or extended partition is 1.
	 *
	 * This function is independent of existing partitions.
	 */
	unsigned int max_primary() const;

	/**
	 * Returns whether a extended partition is supported.
	 *
	 * This function is independent of existing partitions.
	 */
	bool extended_possible() const;

	/**
	 * Highest number for a logical partition. Returns 0 if no logical
	 * partition is possible.
	 *
	 * Lowest number for a logical partition is 5.
	 *
	 * This function is independent of existing partitions.
	 */
	unsigned int max_logical() const;

	unsigned int num_primary() const;
	bool has_extended() const;
	unsigned int num_logical() const;

	/**
	 * Get the partitions of the partition table. This includes primary, extended and
	 * logical partitions.
	 */
	std::vector<Partition*> get_partitions();

	/**
	 * @copydoc get_partitions()
	 */
	std::vector<const Partition*> get_partitions() const;

	/**
	 * @throw Exception
	 */
	Partition* get_partition(const std::string& name);

	/**
	 * Returns the extended partition of the partition table. Throws if no
	 * extended partition exists.
	 *
	 * @throw Exception
	 */
	const Partition* get_extended() const;

	/**
	 * Return the partitionable of the partition table.
	 *
	 * @throw Exception
	 */
	Partitionable* get_partitionable();

	/**
	 * @copydoc get_partitionable()
	 */
	const Partitionable* get_partitionable() const;

	Alignment get_alignment(AlignType align_type = AlignType::OPTIMAL) const;

	/**
	 * @throw Exception
	 */
	std::vector<PartitionSlot> get_unused_partition_slots(AlignPolicy align_policy = AlignPolicy::ALIGN_START_KEEP_END,
							      AlignType align_type = AlignType::OPTIMAL) const;

	/**
	 * region is sector-based.
	 *
	 * @throw AlignError
	 */
	Region align(const Region& region, AlignPolicy align_policy = AlignPolicy::ALIGN_START_AND_END,
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

	/**
	 * Returns whether the no-automount flag is supported on partitions on
	 * the partition table. So far true for GPT if parted 3.6 is available.
	 *
	 * See PartitionTable::is_partition_no_automount_flag_supported()
	 *
	 * @throw Exception
	 */
	bool is_partition_no_automount_flag_supported() const;

	/**
	 * Returns whether the partition id is supported on the partition table.
	 *
	 * This function does not consider the partition type (primary, extended or
	 * logical) so further restriction may apply, e.g. ID_EXTENDED is not allowed for
	 * primary partitions.
	 *
	 * Starting with libstorage-ng version 4.5.47 this function depends on the parted
	 * version and operating system flavour.
	 */
	bool is_partition_id_supported(unsigned int id) const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	PartitionTable(Impl* impl);
	ST_NO_SWIG PartitionTable(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a PartitionTable.
     *
     * @throw NullPointerException
     */
    bool is_partition_table(const Device* device);

    /**
     * Converts pointer to Device to pointer to PartitionTable.
     *
     * @return Pointer to PartitionTable.
     * @throw DeviceHasWrongType, NullPointerException
     */
    PartitionTable* to_partition_table(Device* device);

    /**
     * @copydoc to_partition_table(Device*)
     */
    const PartitionTable* to_partition_table(const Device* device);

}

#endif
