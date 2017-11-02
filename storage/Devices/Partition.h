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


#ifndef STORAGE_PARTITION_H
#define STORAGE_PARTITION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    class PartitionTable;
    class Partitionable;


    //! Partition type (primary, extended, logical)
    enum class PartitionType {
	PRIMARY, EXTENDED, LOGICAL
    };

    std::string get_partition_type_name(PartitionType partition_type);


    /**
     * Enum with values used as partition ids.
     *
     * For MS-DOS the value of the enum is equal to the partition type
     * byte. For GPT and DASD the value has no special meaning.
     *
     * For details about the ids see e.g.
     * https://en.wikipedia.org/wiki/Partition_type and
     * https://en.wikipedia.org/wiki/GUID_Partition_Table.
     */
    enum IdNum : unsigned int {

	/** Only for MS-DOS. */
	ID_DOS12 = 0x01,

	/** Only for MS-DOS. */
	ID_DOS16 = 0x06,

	/** Only for MS-DOS. */
	ID_DOS32 = 0x0c,

	/** Only for MS-DOS. */
	ID_NTFS = 0x07,

	/** Only for MS-DOS. */
	ID_EXTENDED = 0x0f,

	/** For MS-DOS and GPT. */
	ID_DIAG = 0x12,

	/** PPC PReP Boot partition, for MS-DOS and GPT. */
	ID_PREP = 0x41,

	/** For MS-DOS, GPT, DASD and implicit. */
	ID_LINUX = 0x83,

	/** Swap partition, for MS-DOS, GPT and DASD. */
	ID_SWAP = 0x82,

	/** LVM partition, for MS-DOS, GPT and DASD. */
	ID_LVM = 0x8e,

	/** RAID partition, for MS-DOS, GPT and DASD. */
	ID_RAID = 0xfd,

	/** EFI System Partition, for MS-DOS and GPT. */
	ID_ESP = 0xef,

	/** Only set during probing. For MS-DOS, GPT, DASD and implicit. */
	ID_UNKNOWN = 0x100,

	/** BIOS boot partition (https://en.wikipedia.org/wiki/BIOS_boot_partition),
	    only for GPT. */
	ID_BIOS_BOOT = 0x101,

	/** Windows basic data partition, only for GPT. */
	ID_WINDOWS_BASIC_DATA = 0x102,

	/** Microsoft reserved partition, only for GPT. */
	ID_MICROSOFT_RESERVED = 0x103

    };


    //! A partition of a Partitionable, e.g. Disk or Md.
    class Partition : public BlkDevice
    {
    public:

	/**
	 * region is sector-based.
	 */
	static Partition* create(Devicegraph* devicegraph, const std::string& name,
				 const Region& region, PartitionType type);
	static Partition* load(Devicegraph* devicegraph, const xmlNode* node);

	unsigned int get_number() const;

	PartitionType get_type() const;

	/**
	 * @throw Exception
	 */
	void set_type(PartitionType type);

	unsigned int get_id() const;

	/**
	 * @throw Exception
	 */
	void set_id(unsigned int id);

	/**
	 * Query the boot flag of the partition.
	 */
	bool is_boot() const;

	/**
	 * Set the boot flag of the partition. Only supported on Msdos.
	 *
	 * Notes:
	 *
	 * 1. To be
	 * [standard-conformant](https://en.wikipedia.org/wiki/Master_boot_record),
	 * setting the boot flag on a partition clears the boot flag on all
	 * other partitions of the partition table.
	 *
	 * 2. Partitions on GPT have no boot flag, "set <nr> boot on" with
	 * parted on GPT partitions only sets the partition type to EFI System
	 * Partition.
	 *
	 * @throw Exception
	 */
	void set_boot(bool boot);

	/**
	 * Query the legacy boot flag of the partition.
	 */
	bool is_legacy_boot() const;

	/**
	 * Set the legacy boot flag of the partition. Only supported on Gpt.
	 *
	 * @throw Exception
	 */
	void set_legacy_boot(bool legacy_boot);

	const PartitionTable* get_partition_table() const;
	const Partitionable* get_partitionable() const;

	/**
	 * Returns the unused region surrounding the partition (including the
	 * partition itself). The returned region is not aligned.
	 *
	 * @throw Exception
	 */
	Region get_unused_surrounding_region() const;

	/**
	 * Find a Partition by its name. Only the name returned by get_name() is considered.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Partition* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const Partition* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Partition* clone() const override;

    protected:

	Partition(Impl* impl);

    };


    bool is_partition(const Device* device);

    /**
     * Converts pointer to Device to pointer to Partition.
     *
     * @return Pointer to Partition.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Partition* to_partition(Device* device);

    /**
     * @copydoc to_partition(Device*)
     */
    const Partition* to_partition(const Device* device);

}

#endif
