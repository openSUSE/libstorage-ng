/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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


    enum IdNum : unsigned int {
	ID_DOS12 = 0x01, ID_DOS16 = 0x06, ID_DOS32 = 0x0c, ID_NTFS = 0x07,
	ID_EXTENDED = 0x0f, ID_PPC_PREP = 0x41, ID_LINUX = 0x83, ID_SWAP = 0x82,
	ID_LVM = 0x8e, ID_EFI = 0xef, ID_RAID = 0xfd, ID_APPLE_OTHER = 0x101,
	ID_APPLE_HFS = 0x102, ID_GPT_BOOT = 0x103, ID_GPT_SERVICE = 0x104,
	ID_GPT_MSFTRES = 0x105, ID_APPLE_UFS = 0x106, ID_GPT_BIOS = 0x107,
	ID_GPT_PREP = 0x108
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
	 * Set the boot flag of the partition. Only supported on Msdos. Note:
	 * The boot flag known in parted for Gpt sets the partition id to ESP
	 * and is not a discrete flag.
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
