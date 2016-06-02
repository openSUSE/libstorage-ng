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
    class LvmPv;
    class LvmLv;


    //! A Volume Group of the Logical Volume Manager (LVM).
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

	/**
	 * Create a logical volume with name lv_name in the volume group.
	 */
	LvmLv* create_lvm_lv(const std::string& lv_name);

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
