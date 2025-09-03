/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2024] SUSE LLC
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
    class SystemInfo;
    class Arch;


    /**
     * Enum with partition types.
     */
    enum class PartitionType {

	/**
	 * Primary partition. For all partition table types.
	 *
	 * The type PRIMARY is also used for partitions on partition tables
	 * that have no partition type.
	 */
	PRIMARY,

	/**
	 * Extended partition. Only for MS-DOS.
	 */
	EXTENDED,

	/**
	 * Logical partition. Only for MS-DOS.
	 */
	LOGICAL

    };


    /**
     * Convert the PartitionType partition_type to a string.
     *
     * @see PartitionType
     */
    std::string get_partition_type_name(PartitionType partition_type);


    /**
     * Enum with values used as partition ids.
     *
     * For MS-DOS the value of the enum is equal to the partition type byte.
     * For GPT, DASD and Implicit the value has no special meaning.
     *
     * For MS-DOS any value between 0 and 255 is possible, while for the other
     * partition tables only those specified are possible.
     *
     * For details about the ids see e.g.
     * https://en.wikipedia.org/wiki/Partition_type,
     * https://en.wikipedia.org/wiki/GUID_Partition_Table and
     * https://uapi-group.org/specifications/specs/discoverable_partitions_specification/.
     */
    enum IdNum : unsigned int {

	/** Only for MS-DOS. */
	ID_DOS12 = 0x01,

	/** Only for MS-DOS. */
	ID_DOS16 = 0x06,

	/** Only for MS-DOS. */
	ID_NTFS = 0x07,

	/** Only for MS-DOS. */
	ID_DOS32 = 0x0c,

	/** Only for MS-DOS. */
	ID_EXTENDED = 0x0f,

	/** For MS-DOS and GPT. */
	ID_DIAG = 0x12,

	/** PPC PReP Boot partition, for MS-DOS and GPT. */
	ID_PREP = 0x41,

	/** Swap partition, for MS-DOS, GPT and DASD. */
	ID_SWAP = 0x82,

	/** For MS-DOS, GPT, DASD and implicit. Unfortunately with old parted versions
	    this is the fallback for unknown ids on GPT. */
	ID_LINUX = 0x83,

	/** Intel Rapid Start Technology, for MS-DOS and GPT. */
	ID_IRST = 0x84,

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
	ID_BIOS_BOOT,

	/** Windows basic data partition, only for GPT. */
	ID_WINDOWS_BASIC_DATA,

	/** Microsoft reserved partition, only for GPT. */
	ID_MICROSOFT_RESERVED,

	/** Linux Home, only for GPT. Requires parted 3.5 or higher. */
	ID_LINUX_HOME,

	/** Linux Server Data, only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_SERVER_DATA,

	/** Linux Root Partition (arm), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_ARM,

	/** Linux Root Partition (aarch64), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_AARCH64,

	/** Linux Root Partition (ppc), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_PPC32,

	/** Linux Root Partition (ppc64be), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_PPC64BE,

	/** Linux Root Partition (ppc64le), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_PPC64LE,

	/** Linux Root Partition (riscv32, only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_RISCV32,

	/** Linux Root Partition (riscv64), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_RISCV64,

	/** Linux Root Partition (s390), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_S390,

	/** Linux Root Partition (s390x), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_S390X,

	/** Linux Root Partition (x86), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_X86,

	/** Linux Root Partition (x86_64), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_ROOT_X86_64,

	/** Linux USR Partition (arm), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_ARM,

	/** Linux USR Partition (aarch64), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_AARCH64,

	/** Linux USR Partition (ppc), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_PPC32,

	/** Linux USR Partition (ppc64be), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_PPC64BE,

	/** Linux USR Partition (ppc64le), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_PPC64LE,

	/** Linux USR Partition (riscv32), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_RISCV32,

	/** Linux USR Partition (riscv64), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_RISCV64,

	/** Linux USR Partition (s390), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_S390,

	/** Linux USR Partition (s390x), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_S390X,

	/** Linux USR Partition (x86), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_X86,

	/** Linux USR Partition (x86_64), only for GPT. Requires SUSE parted 3.5 or higher. */
	ID_LINUX_USR_X86_64,

    };


    /**
     * Convert the IdNum partition_id to a string.
     *
     * @note An empty string could be returned (e.g., when the given partition id is unknown).
     *
     * @see IdNum
     */
    std::string get_partition_id_name(IdNum partition_id);


    /**
     * Check whether the id is a Linux partition id. Thus true for e.g. ID_LINUX,
     * ID_LINUX_HOME, ID_LINUX_ROOT_X86_64 and ID_RAID and false for e.g. ID_ESP and
     * ID_PREP.
     *
     * @see IdNum
     */
    bool is_linux_partition_id(IdNum id);


    /**
     * Enum with categories for Linux partitions.
     */
    enum class LinuxPartitionIdCategory
    {
	ROOT, USR
    };


    /**
     * Check whether the id is a Linux partition id of the specified category.
     *
     * @see IdNum, LinuxPartitionIdCategory
     */
    bool is_linux_partition_id(IdNum id, LinuxPartitionIdCategory linux_partition_id_category);


    /**
     * Get the partition id for the Linux partition of the given category (root, usr, ...)
     * depending on the architecture.
     *
     * @see IdNum, LinuxPartitionIdCategory
     *
     * @throw Exception
     */
    IdNum get_linux_partition_id(LinuxPartitionIdCategory linux_partition_id_category, const Arch& arch);


    /**
     * Get the partition id for the Linux partition of the given category (root, usr, ...)
     * depending on the architecture.
     *
     * @see IdNum, LinuxPartitionIdCategory
     *
     * @throw Exception
     */
    IdNum get_linux_partition_id(LinuxPartitionIdCategory linux_partition_id_category, SystemInfo& system_info);


    /**
     * A partition of a Partitionable, e.g. Disk or Md.
     */
    class Partition : public BlkDevice
    {
    public:

	/**
	 * Create a device of type Partition. Region is sector-based. Usually this
	 * function is not called directly. Instead PartitionTable::create_partition() is
	 * called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Partition* create(Devicegraph* devicegraph, const std::string& name,
				 const Region& region, PartitionType type);

	static Partition* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the partition number.
	 */
	unsigned int get_number() const;

	/**
	 * Get the partition type.
	 *
	 * @see PartitionType
	 */
	PartitionType get_type() const;

	/**
	 * Set the partition type.
	 *
	 * @see PartitionType
	 *
	 * @throw Exception
	 */
	void set_type(PartitionType type);

	/**
	 * Get the partition id.
	 *
	 * @see IdNum
	 */
	unsigned int get_id() const;

	/**
	 * Set the partition id.
	 *
	 * @see IdNum, PartitionTable::is_partition_id_supported()
	 *
	 * @throw Exception
	 */
	void set_id(unsigned int id);

	/**
	 * Query the boot flag of the partition.
	 */
	bool is_boot() const;

	/**
	 * Set the boot flag of the partition. Only supported on MS-DOS.
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
	 * @see PartitionTable::is_partition_boot_flag_supported()
	 *
	 * @throw Exception
	 */
	void set_boot(bool boot);

	/**
	 * Query the legacy boot flag of the partition.
	 */
	bool is_legacy_boot() const;

	/**
	 * Set the legacy boot flag of the partition. Only supported on GPT.
	 *
	 * @see PartitionTable::is_partition_legacy_boot_flag_supported()
	 *
	 * @throw Exception
	 */
	void set_legacy_boot(bool legacy_boot);

	/**
	 * Query the no-automount flag of the partition.
	 */
	bool is_no_automount() const;

	/**
	 * Set the no-automount flag of the partition. Only supported on GPT. Requires
	 * parted 3.6.
	 *
	 * The flag is called "no_automount" by parted and "no-auto" by systemd, see
	 * https://www.freedesktop.org/wiki/Specifications/DiscoverablePartitionsSpec/.
	 *
	 * @see PartitionTable::is_partition_no_automount_flag_supported()
	 *
	 * @throw Exception
	 */
	void set_no_automount(bool no_automount);

	/**
	 * Get the partition label. Can be empty. Only available for partitions on GPT.
	 *
	 * In the UEFI spec this is called name instead of label. But many tools,
	 * e.g. udev and blkid, use label. Also in fstab PARTLABEL= is used.
	 */
	const std::string& get_label() const;

	/**
	 * Set the partition label.
	 *
	 * @see get_label()
	 */
	void set_label(const std::string& label);

	/**
	 * Get the partition UUID. Only available for partitions on GPT. Experimental.
	 */
	const std::string& get_uuid() const;

	/**
	 * Return the partition table the partition belongs to.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	PartitionTable* get_partition_table();

	/**
	 * @copydoc get_partition_table()
	 */
	const PartitionTable* get_partition_table() const;

	/**
	 * Return the partitionable the partition belongs to.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	Partitionable* get_partitionable();

	/**
	 * @copydoc get_partitionable()
	 */
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

	/**
	 * Compare (less than) two Partitions by number.
	 *
	 * @see get_number()
	 */
	static bool compare_by_number(const Partition* lhs, const Partition* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Partition* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Partition(Impl* impl);
	ST_NO_SWIG Partition(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Partition.
     *
     * @throw NullPointerException
     */
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
