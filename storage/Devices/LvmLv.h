/*
 * Copyright (c) [2016-2020] SUSE LLC
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
	WRITECACHE
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


    /**
     * A Logical Volume of the Logical Volume Manager (LVM).
     */
    class LvmLv : public BlkDevice
    {

    public:

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
	 * Get logical volume name. This is different from get_name().
	 */
	const std::string& get_lv_name() const;
	void set_lv_name(const std::string& lv_name);

	LvType get_lv_type() const;

	unsigned int get_stripes() const;

	/**
	 * Set the number of stripes. The size of the LV must be a multiple of
	 * the number of stripes and the stripe size. Thin LV cannot be
	 * striped.
	 *
	 * @throw Exception
	 */
	void set_stripes(unsigned int stripes);

	unsigned long long get_stripe_size() const;

	/**
	 * @throw Exception
	 */
	void set_stripe_size(unsigned long long stripe_size);

	unsigned long long get_chunk_size() const;

	/**
	 * Set the chunk size. Only thin pools can have a chunk size.
	 *
	 * @throw Exception
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
	 * thin pool. Only supported lv_type is THIN.
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
	 */
	static bool compare_by_lv_name(const LvmLv* lhs, const LvmLv* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmLv* clone() const override;

    protected:

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
