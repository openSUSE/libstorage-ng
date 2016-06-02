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


#ifndef STORAGE_LVM_LV_H
#define STORAGE_LVM_LV_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{
    class LvmVg;


    //! A Logical Volume of the Logical Volume Manager (LVM).
    class LvmLv : public BlkDevice
    {
    public:

	static LvmLv* create(Devicegraph* devicegraph, const std::string& vg_name, const std::string& lv_name);
	static LvmLv* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get logical volume name. This is different from get_name().
	 */
	const std::string& get_lv_name() const;
	void set_lv_name(const std::string& lv_name);

	virtual void check() const override;

	/**
	 * Return volume group this logical volume belongs to.
	 */
	const LvmVg* get_lvm_vg() const;

	static LvmLv* find_by_uuid(Devicegraph* devicegraph, const std::string& uuid);
	static const LvmLv* find_by_uuid(const Devicegraph* devicegraph, const std::string& uuid);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmLv* clone() const override;

    protected:

	LvmLv(Impl* impl);

    };


    bool is_lvm_lv(const Device* device);

    LvmLv* to_lvm_lv(Device* device);
    const LvmLv* to_lvm_lv(const Device* device);

}

#endif
