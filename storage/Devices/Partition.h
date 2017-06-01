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
     * For MSDOS the value of the enum is equal to the partition type
     * byte. For GPT and DASD the value has no special meaning.
     *
     * For details about the ids see e.g.
     * https://en.wikipedia.org/wiki/Partition_type and
     * https://en.wikipedia.org/wiki/GUID_Partition_Table.
     */
    enum IdNum : unsigned int {

	/** Only for MSDOS. */
	ID_DOS12 = 0x01,

	/** Only for MSDOS. */
	ID_DOS16 = 0x06,

	/** Only for MSDOS. */
	ID_DOS32 = 0x0c,

	/** Only for MSDOS. */
	ID_NTFS = 0x07,

	/** Only for MSDOS. */
	ID_EXTENDED = 0x0f,

	/** For MSDOS and GPT. */
	ID_DIAG = 0x12,

	/** PPC PReP Boot partition, for MSDOS and GPT. */
	ID_PREP = 0x41,

	/** For MSDOS and GPT. */
	ID_LINUX = 0x83,

	/** Swap partition, for MSDOS and GPT, but GPT only when the
	    partition is created. */
	ID_SWAP = 0x82,

	/** LVM partition, for MSDOS and GPT. */
	ID_LVM = 0x8e,

	/** RAID partition, for MSDOS and GPT. */
	ID_RAID = 0xfd,

	/** EFI System Partition, for MSDOS and GPT. */
	ID_ESP = 0xef,

	/** Only set during probing. For MSDOS and GPT. */
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
	void set_type(PartitionType type);

	unsigned int get_id() const;
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
	 */
	void set_boot(bool boot);

	/**
	 * Query the legacy boot flag of the partition.
	 */
	bool is_legacy_boot() const;

	/**
	 * Set the legacy boot flag of the partition. Only supported on Gpt.
	 */
	void set_legacy_boot(bool legacy_boot);

	const PartitionTable* get_partition_table() const;
	const Partitionable* get_partitionable() const;

	static Partition* find_by_name(Devicegraph* devicegraph, const std::string& name);
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

    Partition* to_partition(Device* device);
    const Partition* to_partition(const Device* device);

}

#endif
