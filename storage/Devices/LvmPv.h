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

	static LvmPv* create(Devicegraph* devicegraph);
	static LvmPv* load(Devicegraph* devicegraph, const xmlNode* node);

	static std::vector<LvmPv*> get_all(Devicegraph* devicegraph);
	static std::vector<const LvmPv*> get_all(const Devicegraph* devicegraph);

	bool has_blk_device() const;

	BlkDevice* get_blk_device();
	const BlkDevice* get_blk_device() const;

	/**
	 * Get the size of the PV usable for extents in bytes. Can be
	 * 0 (if pe_start >= size of blk device).
	 *
	 * @throw Exception
	 */
	unsigned long long get_usable_size() const;

	bool has_lvm_vg() const;
	LvmVg* get_lvm_vg();
	const LvmVg* get_lvm_vg() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmPv* clone() const override;

    protected:

	LvmPv(Impl* impl);

    };


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
