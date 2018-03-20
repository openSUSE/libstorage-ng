/*
 * Copyright (c) [2004-2010] Novell, Inc.
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


#ifndef STORAGE_FREE_INFO_H
#define STORAGE_FREE_INFO_H


#include <libxml/tree.h>
#include <ostream>


// TODO find better name for file and class


namespace storage
{

    /**
     * Resize blockers.
     *
     * Some resize operations are not supported by the system (e.g. shrink LVM
     * thin pool) while others are not supported by the library (e.g. resize
     * extended partition).
     */
    enum : uint32_t
    {

	/**
	 * The device does not support resizing. This is the case for Disks,
	 * Dasds, Mds, DmRaids, Multipaths, PartitionTables, LvmVgs, Bcaches,
	 * BcacheCsets, BtrfsSubvolumes, Nfses MountPoints, so basically
	 * everything except Partitions and LvmLvs.
	 *
	 * Blocks shrink and grow.
	 */
	RB_RESIZE_NOT_SUPPORTED_BY_DEVICE = 1 << 0,

	/**
	 * Several limitations were combined, e.g. limitations from partition
	 * and filesystem, and the result does not allow any resize.
	 *
	 * Blocks shrink and grow.
	 */
	RB_MIN_MAX_ERROR = 1 << 1,

	/**
	 * The filesystem does not support shrink.
	 *
	 * Blocks shrink.
	 */
	RB_SHRINK_NOT_SUPPORTED_BY_FILESYSTEM = 1 << 2,

	/**
	 * The filesystem does not support grow.
	 *
	 * Blocks grow.
	 */
	RB_GROW_NOT_SUPPORTED_BY_FILESYSTEM = 1 << 3,

	/**
	 * The filesystem seems to be inconsistent or checking filesystem
	 * consistency failed.
	 *
	 * Blocks shrink and grow.
	 */
	RB_FILESYSTEM_INCONSISTENT = 1 << 4,

	/**
	 * The filesystem has already the minimal possible size.
	 *
	 * Blocks shrink.
	 */
	RB_MIN_SIZE_FOR_FILESYSTEM = 1 << 5,

	/**
	 * The filesystem has already the maximal possible size.
	 *
	 * Blocks grow.
	 */
	RB_MAX_SIZE_FOR_FILESYSTEM = 1 << 6,

	/**
	 * The filesystem is full and cannot be shrunk.
	 *
	 * Blocks shrink.
	 */
	RB_FILESYSTEM_FULL = 1 << 7,

	/**
	 * No space behind partition. There can be several reasons: 1. other
	 * partition, 2. end of disk, 3. end of extended partition, 4. space
	 * not addressable by partition table.
	 *
	 * Blocks grow.
	 */
	RB_NO_SPACE_BEHIND_PARTITION = 1 << 8,

	/**
	 * The partition has already the minimal possible size.
	 *
	 * Blocks shrink.
	 */
	RB_MIN_SIZE_FOR_PARTITION = 1 << 9,

	/**
	 * Extended partitions cannot be resized.
	 *
	 * Blocks shrink and grow.
	 */
	RB_EXTENDED_PARTITION = 1 << 10,

	/**
	 * Partition on an implicit partition table cannot be resized.
	 *
	 * Blocks shrink and grow.
	 */
	RB_ON_IMPLICIT_PARTITION_TABLE = 1 << 11,

	/**
	 * Shrink of LVM logical volume of this type not supported, e.g. thin pools and raids.
	 *
	 * Blocks shrink.
	 */
	RB_SHRINK_NOT_SUPPORTED_FOR_LVM_LV_TYPE = 1 << 12,

	/**
	 * Resize of LVM logical volume of this type not supported, e.g. raids.
	 *
	 * Blocks shrink and grow.
	 */
	RB_RESIZE_NOT_SUPPORTED_FOR_LVM_LV_TYPE = 1 << 13,

	/**
	 * No space left in LVM volume group.
	 *
	 * Blocks grow.
	 */
	RB_NO_SPACE_IN_LVM_VG = 1 << 14,

	/**
	 * The LVM logical volume has already the minimal possible size.
	 *
	 * Blocks shrink.
	 */
	RB_MIN_SIZE_FOR_LVM_LV = 1 << 15,

	/**
	 * LVM thin logical volume has already maximal size.
	 *
	 * Blocks grow.
	 */
	RB_MAX_SIZE_FOR_LVM_LV_THIN = 1 << 16

    };


    class ResizeInfo
    {
    public:

	ResizeInfo(bool resize_ok, uint32_t reasons, unsigned long long min_size, unsigned long long max_size);

	ResizeInfo(bool resize_ok, uint32_t reasons);

	/**
	 * min_size = max(min_size, extra_resize_info.min_size)
	 * max_size = min(max_size, extra_resize_info.max_size)
	 *
	 * Use e.g. to combine limits from filesystem with limits from partition.
	 */
	void combine(ResizeInfo extra_resize_info);

	void combine_min(unsigned long long extra_min_size);

	void combine_max(unsigned long long extra_max_size);

	void combine_block_size(unsigned long long extra_block_size);

	/**
	 * min_size += offset
	 * max_size += offset
	 *
	 * Use e.g. to shift limits from filesystem by metadata of LUKS.
	 */
	void shift(unsigned long long offset);

	bool resize_ok;

	uint32_t reasons;

	unsigned long long min_size;
	unsigned long long max_size;

	unsigned long long block_size;

	friend std::ostream& operator<<(std::ostream& out, const ResizeInfo& resize_info);

    public:

	ResizeInfo(const xmlNode* node);

	void save(xmlNode* node) const;

    private:

	void check();

    };


    class ContentInfo
    {
    public:

	ContentInfo(bool is_windows, bool is_efi, unsigned num_homes);
	ContentInfo();

	bool is_windows;
	bool is_efi;
	unsigned num_homes;

	friend std::ostream& operator<<(std::ostream& out, const ContentInfo& content_info);

    public:

	ContentInfo(const xmlNode* node);

	void save(xmlNode* node) const;

    };


    class SpaceInfo
    {
    public:

	SpaceInfo(unsigned long long size, unsigned long long used);

	unsigned long long size;
	unsigned long long used;

	std::string get_size_string() const;

	friend std::ostream& operator<<(std::ostream& out, const SpaceInfo& space_info);

    public:

	SpaceInfo(const xmlNode* node);

	void save(xmlNode* node) const;

    };

}


#endif
