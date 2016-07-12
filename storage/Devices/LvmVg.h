/*
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


#ifndef STORAGE_LVM_VG_H
#define STORAGE_LVM_VG_H


#include "storage/Devices/Device.h"


namespace storage
{
    class BlkDevice;
    class LvmPv;
    class LvmLv;


    struct InvalidExtentSize : public Exception
    {
	InvalidExtentSize(unsigned long long extent_size);
    };


    /**
     * A Volume Group of the Logical Volume Manager (LVM).
     */
    class LvmVg : public Device
    {
    public:

	static LvmVg* create(Devicegraph* devicegraph, const std::string& vg_name);
	static LvmVg* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get volume group name. This is different from get_name().
	 */
	const std::string& get_vg_name() const;
	void set_vg_name(const std::string& vg_name);

	virtual void check() const override;

	const Region& get_region() const;

	unsigned long long get_size() const;

	/**
	 * Returns the extent size of the volume group.
	 *
	 * @return extent size
	 */
	unsigned long long get_extent_size() const;

	/**
	 * Set the extent size of the volume group. This can modify the size
	 * of the logical volumes.
	 *
	 * @throw InvalidExtentSize
	 */
	void set_extent_size(unsigned long long extent_size);

	/**
	 * Adds a block device as a physical volume to the volume group. If
	 * there is not a physical volume on the block device it will be
	 * created.
	 */
	LvmPv* add_pv(BlkDevice* blk_device);

	/**
	 * Removes a block device from the volume group. The physical volume
	 * on the block device will the deleted.
	 */
	void remove_pv(BlkDevice* blk_device);

	/**
	 * Create a logical volume with name lv_name in the volume group.
	 */
	LvmLv* create_lvm_lv(const std::string& lv_name, unsigned long long size);

	/**
	 * Delete a logical volume in the volume group. Also deletes all
	 * descendants of the logical volume.
	 */
	void delete_lvm_lv(LvmLv* lvm_lv);

	/**
	 * Sorted by vg_name.
	 */
	static std::vector<LvmVg*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const LvmVg*> get_all(const Devicegraph* devicegraph);

	std::vector<LvmPv*> get_lvm_pvs();
	std::vector<const LvmPv*> get_lvm_pvs() const;

	LvmLv* get_lvm_lv(const std::string& lv_name);

	std::vector<LvmLv*> get_lvm_lvs();
	std::vector<const LvmLv*> get_lvm_lvs() const;

	static LvmVg* find_by_uuid(Devicegraph* devicegraph, const std::string& uuid);
	static const LvmVg* find_by_uuid(const Devicegraph* devicegraph, const std::string& uuid);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmVg* clone() const override;

    protected:

	LvmVg(Impl* impl);

    };


    bool is_lvm_vg(const Device* device);

    LvmVg* to_lvm_vg(Device* device);
    const LvmVg* to_lvm_vg(const Device* device);

}

#endif
