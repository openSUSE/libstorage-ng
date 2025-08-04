/*
 * Copyright (c) [2016-2021] SUSE LLC
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


#ifndef STORAGE_LVM_LV_H
#define STORAGE_LVM_LV_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devicegraph.h"


namespace storage
{

    class LvmVg;


    /**
     * LVM logical volume types, see lvs(8).
     */
    enum class LvType
    {
	/**
	 * A logical volume of unknown type.
	 *
	 * Cannot be used as a blk device. Cannot be created by the library.
	 */
	UNKNOWN,

	/**
	 * A linear or striped logical volume.
	 */
	NORMAL,

	/**
	 * A thin-pool logical volume.
	 *
	 * Cannot be used as a blk device.
	 */
	THIN_POOL,

	/**
	 * A thin logical volume.
	 */
	THIN,

	/**
	 * A raid logical volume.
	 *
	 * Cannot be created by the library.
	 */
	RAID,

	/**
	 * A cache-pool logical volume.
	 *
	 * Cannot be used as a blk device. Cannot be created by the library.
	 */
	CACHE_POOL,

	/**
	 * A cache logical volume.
	 *
	 * Cannot be created by the library.
	 */
	CACHE,

	/**
	 * A writecache logical volume.
	 *
	 * Cannot be created by the library.
	 */
	WRITECACHE,

	/**
	 * A thick snapshot. Thin snapshots have the type THIN.
	 *
	 * Cannot be created by the library.
	 */
	SNAPSHOT,

	/**
	 * A mirror volume. This is different from RAID1 in that it
	 * has the mirror log on a separate device.
	 *
	 * Cannot be created by the library.
	 */
	MIRROR
    };


    /**
     * Convert LvType to string.
     *
     * @see LvType
     */
    std::string get_lv_type_name(LvType lv_type);


    class LvmLvNotFoundByLvName : public DeviceNotFound
    {
    public:

	LvmLvNotFoundByLvName(const std::string& lv_name);
    };


    class InvalidStripeSize : public Exception
    {
    public:

	InvalidStripeSize(const std::string& msg);
    };


    class InvalidChunkSize : public Exception
    {
    public:

	InvalidChunkSize(const std::string& msg);
    };


    /**
     * A Logical Volume of the Logical Volume Manager (LVM).
     */
    class LvmLv : public BlkDevice
    {

    public:

	/**
	 * Create a device of type LvmLv. Usually this function is not called
	 * directly. Instead LvmVg::create_lvm_lv() or LvmLv::create_lvm_lv() are called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static LvmLv* create(Devicegraph* devicegraph, const std::string& vg_name,
			     const std::string& lv_name, LvType lv_type);

	static LvmLv* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all LvmLvs.
	 */
	static std::vector<LvmLv*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const LvmLv*> get_all(const Devicegraph* devicegraph);

	/**
	 * Get the logical volume name. This does not include the name
	 * of the volume group.  The logical volume name is different
	 * from block device name returned by get_name().
	 *
	 * For the logical volume name "root" the block device name
	 * maybe be "/dev/system/root".
	 */
	const std::string& get_lv_name() const;

	/**
	 * Set the logical volume name. The library does not support to rename logical
	 * volumes on disk.
	 *
	 * @see get_lv_name()
	 */
	void set_lv_name(const std::string& lv_name);

	/**
	 * Check whether a (new and public) logical volume name is valid. Does not check
	 * for collisions.
	 */
	static bool is_valid_lv_name(const std::string& lv_name);

	/**
	 * Return the logical volume type.
	 *
	 * @see LvType
	 */
	LvType get_lv_type() const;

	/**
	 * Get the number of stripes.
	 */
	unsigned int get_stripes() const;

	/**
	 * Set the number of stripes. The size of the LV must be a multiple of
	 * the number of stripes and the stripe size. Thin LVs cannot be
	 * striped.
	 *
	 * @throw Exception
	 */
	void set_stripes(unsigned int stripes);

	/**
	 * Get the stripe size.
	 */
	unsigned long long get_stripe_size() const;

	/**
	 * Set the stripe size.
	 *
	 * @see set_stripes(unsigned int)
	 *
	 * @throw InvalidStripeSize, Exception
	 */
	void set_stripe_size(unsigned long long stripe_size);

	/**
	 * Get the chunk size. Only thin pools can have a chunk size.
	 */
	unsigned long long get_chunk_size() const;

	/**
	 * Set the chunk size. Only thin pools can have a chunk size.
	 *
	 * @throw InvalidChunkSize, Exception
	 */
	void set_chunk_size(unsigned long long chunk_size);

	/**
	 * Return volume group this logical volume belongs to.
	 *
	 * @throw Exception
	 */
	const LvmVg* get_lvm_vg() const;

	/**
	 * Return the thin pool for a thin logical volume.
	 *
	 * @throw Exception
	 */
	const LvmLv* get_thin_pool() const;

	/**
	 * Check whether the logical volume has snapshots. These can
	 * be either thick or thin snapshots.
	 */
	bool has_snapshots() const;

	/**
	 * Get snapshots of the logical volume.
	 *
	 * @see has_snapshots()
	 */
	std::vector<LvmLv*> get_snapshots();

	/**
	 * @copydoc get_snapshots()
	 */
	std::vector<const LvmLv*> get_snapshots() const;

	/**
	 * Check whether the logical volume has an origin. In other
	 * words, whether it is a snapshot. It can be either a thick
	 * or thin snapshot.
	 *
	 * @see has_snapshots()
	 */
	bool has_origin() const;

	/**
	 * Get the origin of the logical volume if it has one.
	 *
	 * @see has_origin()
	 *
	 * @throw Exception
	 */
	LvmLv* get_origin();

	/**
	 * @copydoc get_origin()
	 */
	const LvmLv* get_origin() const;

	/**
	 * Return the max size in bytes for a new logical volume of type
	 * lv_type. The size may be limited by other parameters, e.g. the
	 * filesystem on it.
	 *
	 * The max size for thin logical volumes is in general theoretic since
	 * a thin pool logical volume should never be overcommited so much.
	 */
	unsigned long long max_size_for_lvm_lv(LvType lv_type) const;

	/**
	 * Create a logical volume with name lv_name and type lv_type in the
	 * thin pool.
	 *
	 * @see LvType
	 *
	 * @throw Exception
	 */
	LvmLv* create_lvm_lv(const std::string& lv_name, LvType lv_type, unsigned long long size);

	/**
	 * @throw Exception
	 */
	LvmLv* get_lvm_lv(const std::string& lv_name);

	std::vector<LvmLv*> get_lvm_lvs();
	std::vector<const LvmLv*> get_lvm_lvs() const;

	/**
	 * Compare (less than) two LvmLvs by lv-name.
	 *
	 * The comparison is locale unaware.
	 *
	 * @see get_lv_name()
	 */
	static bool compare_by_lv_name(const LvmLv* lhs, const LvmLv* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmLv* clone() const override;

	LvmLv(Impl* impl);

    };


    /**
     * Checks whether device points to a LvmLv.
     *
     * @throw NullPointerException
     */
    bool is_lvm_lv(const Device* device);

    /**
     * Converts pointer to Device to pointer to LvmLv.
     *
     * @return Pointer to LvmLv.
     * @throw DeviceHasWrongType, NullPointerException
     */
    LvmLv* to_lvm_lv(Device* device);

    /**
     * @copydoc to_lvm_lv(Device*)
     */
    const LvmLv* to_lvm_lv(const Device* device);

}

#endif
