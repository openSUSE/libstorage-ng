/*
 * Copyright (c) [2016-2019] SUSE LLC
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


#ifndef STORAGE_LVM_PV_H
#define STORAGE_LVM_PV_H


#include "storage/Devices/Device.h"


namespace storage
{
    class BlkDevice;
    class LvmVg;


    class LvmPv : public Device
    {
    public:

	/**
	 * Create a device of type LvmPv. Usually this function is not called
	 * directly. Instead LvmVg::add_lvm_pv() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static LvmPv* create(Devicegraph* devicegraph);

	static LvmPv* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all LvmPvs.
	 */
	static std::vector<LvmPv*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const LvmPv*> get_all(const Devicegraph* devicegraph);

	/**
	 * Check whether the physical volume has a block
	 * device. Normally this is the case, but LVM may report block
	 * devices as missing during probing.
	 */
	bool has_blk_device() const;

	/**
	 * Return the block device this physical volume is using.
	 *
	 * @throw Exception
	 */
	BlkDevice* get_blk_device();

	/**
	 * @copydoc get_blk_device()
	 */
	const BlkDevice* get_blk_device() const;

	/**
	 * Get the size of the PV usable for extents in bytes. Can be
	 * 0 (if pe_start >= size of blk device).
	 *
	 * @throw Exception
	 */
	unsigned long long get_usable_size() const;

	/**
	 * Check whether the physical volume is part of a volume group.
	 */
	bool has_lvm_vg() const;

	/**
	 * Return the volume group the physical volume is used in.
	 *
	 * @throw Exception
	 */
	LvmVg* get_lvm_vg();

	/**
	 * @copydoc get_lvm_vg()
	 */
	const LvmVg* get_lvm_vg() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmPv* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	LvmPv(Impl* impl);
	ST_NO_SWIG LvmPv(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a LvmPv.
     *
     * @throw NullPointerException
     */
    bool is_lvm_pv(const Device* device);

    /**
     * Converts pointer to Device to pointer to LvmPv.
     *
     * @return Pointer to LvmPv.
     * @throw DeviceHasWrongType, NullPointerException
     */
    LvmPv* to_lvm_pv(Device* device);

    /**
     * @copydoc to_lvm_pv(Device*)
     */
    const LvmPv* to_lvm_pv(const Device* device);

}

#endif
