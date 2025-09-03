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


#ifndef STORAGE_LVM_VG_H
#define STORAGE_LVM_VG_H


#include "storage/Devices/Device.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devicegraph.h"


namespace storage
{
    class Region;
    class BlkDevice;
    class LvmPv;
    class LvmLv;


    class LvmVgNotFoundByVgName : public DeviceNotFound
    {
    public:

	LvmVgNotFoundByVgName(const std::string& vg_name);
    };


    class InvalidExtentSize : public Exception
    {
    public:

	InvalidExtentSize(unsigned long long extent_size) ST_DEPRECATED;
	InvalidExtentSize(const std::string& msg);
    };


    /**
     * A Volume Group of the Logical Volume Manager (LVM).
     */
    class LvmVg : public Device
    {
    public:

	/**
	 * Create a device of type LvmVg.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static LvmVg* create(Devicegraph* devicegraph, const std::string& vg_name);

	static LvmVg* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get volume group name. This is different from get_name().
	 */
	const std::string& get_vg_name() const;

	/**
	 * Set the volume group name. The library does not support to rename volume groups
	 * on disk.
	 *
	 * @see get_vg_name()
	 */
	void set_vg_name(const std::string& vg_name);

	/**
	 * Check whether a volume group name is valid. Does not check for collisions in
	 * /dev.
	 */
	static bool is_valid_vg_name(const std::string& vg_name);

	/**
	 * Get the region of the volume group. The start of the region is always 0, the
	 * length is the number of extents and the block size in the extent size.
	 */
	const Region& get_region() const;

	/**
	 * Returns the size of the volume group in bytes.
	 */
	unsigned long long get_size() const;

	/**
	 * Returns the size of the volume group as a localised string.
	 *
	 * @see get_size(), byte_to_humanstring()
	 * @return size as string
	 */
	std::string get_size_string() const;

	/**
	 * Returns the extent size of the volume group.
	 *
	 * @return extent size
	 */
	unsigned long long get_extent_size() const;

	/**
	 * Set the extent size of the volume group. This can modify the size
	 * of the logical volumes. Due to rounding the size of logical volumes
	 * can become zero or the volume group can become overcommitted.
	 *
	 * Setting the extent size is unsupported if the volume group is
	 * already created on-disk (it is only supported by LVM for special
	 * cases).
	 *
	 * @throw InvalidExtentSize, Exception
	 */
	void set_extent_size(unsigned long long extent_size);

	/**
	 * Returns the number of extents in the volume group.
	 */
	unsigned long long number_of_extents() const;

	/**
	 * Calculates the number of used extents in the volume group. May
	 * be larger than the number of extents in the volume group.
	 */
	unsigned long long number_of_used_extents() const;

	/**
	 * Calculates the number of free extents in the volume group.
	 */
	unsigned long long number_of_free_extents() const;

	/**
	 * Check whether the volume group is overcommitted. If it is,
	 * Storage::commit() will most likely fail.
	 */
	bool is_overcommitted() const;

	/**
	 * Adds a block device as a physical volume to the volume group. If
	 * there is not a physical volume on the block device it will be
	 * created.
	 *
	 * @throw Exception
	 */
	LvmPv* add_lvm_pv(BlkDevice* blk_device);

	/**
	 * Removes a block device from the volume group. The physical volume
	 * on the block device will the deleted.
	 *
	 * @throw Exception
	 */
	void remove_lvm_pv(BlkDevice* blk_device);

	/**
	 * Return the max size in bytes for a new logical volume of type
	 * lv_type. The size may be limited by other parameters, e.g. the
	 * filesystem on it. The max size also depends on parameters, e.g. the
	 * chunk size for thin pools.
	 */
	unsigned long long max_size_for_lvm_lv(LvType lv_type) const;

	/**
	 * Create a logical volume with name lv_name and type lv_type in the
	 * volume group.
	 *
	 * @see LvType
	 */
	LvmLv* create_lvm_lv(const std::string& lv_name, LvType lv_type,
			     unsigned long long size);

	LvmLv* create_lvm_lv(const std::string& lv_name, unsigned long long size) ST_DEPRECATED;

	/**
	 * Delete a logical volume in the volume group. Also deletes all
	 * descendants of the logical volume.
	 */
	void delete_lvm_lv(LvmLv* lvm_lv);

	/**
	 * Get all LvmVgs.
	 */
	static std::vector<LvmVg*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const LvmVg*> get_all(const Devicegraph* devicegraph);

	/**
	 * Find a volume group by vg-name.
	 *
	 * @throw LvmVgNotFoundByVgName
	 */
	static LvmVg* find_by_vg_name(Devicegraph* devicegraph, const std::string& vg_name);

	/**
	 * @copydoc find_by_vg_name()
	 */
	static const LvmVg* find_by_vg_name(const Devicegraph* devicegraph, const std::string& vg_name);

	/**
	 * Get all physical volumes of the volume group.
	 */
	std::vector<LvmPv*> get_lvm_pvs();

	/**
	 * @copydoc get_lvm_pvs()
	 */
	std::vector<const LvmPv*> get_lvm_pvs() const;

	/**
	 * @throw Exception
	 */
	LvmLv* get_lvm_lv(const std::string& lv_name);

	/**
	 * Get all logical volumes of the volume group that are direct
	 * children of the volume group. E.g. thin logical volumes are
	 * not included here since those are children of the thin
	 * pool.
	 */
	std::vector<LvmLv*> get_lvm_lvs();

	/**
	 * @copydoc get_lvm_lvs()
	 */
	std::vector<const LvmLv*> get_lvm_lvs() const;

	/**
	 * Compare (less than) two LvmVgs by vg-name.
	 *
	 * The comparison is locale unaware.
	 *
	 * @see get_vg_name()
	 */
	static bool compare_by_vg_name(const LvmVg* lhs, const LvmVg* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmVg* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	LvmVg(Impl* impl);
	ST_NO_SWIG LvmVg(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a LvmVg.
     *
     * @throw NullPointerException
     */
    bool is_lvm_vg(const Device* device);

    /**
     * Converts pointer to Device to pointer to LvmVg.
     *
     * @return Pointer to LvmVg.
     * @throw DeviceHasWrongType, NullPointerException
     */
    LvmVg* to_lvm_vg(Device* device);

    /**
     * @copydoc to_lvm_vg(Device*)
     */
    const LvmVg* to_lvm_vg(const Device* device);

}

#endif
