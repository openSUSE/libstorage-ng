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


#ifndef STORAGE_LVM_PV_H
#define STORAGE_LVM_PV_H


#include "storage/Devices/Device.h"


namespace storage
{

    class LvmPv : public Device
    {
    public:

	static LvmPv* create(Devicegraph* devicegraph);
	static LvmPv* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual void check() const override;

	static std::vector<LvmPv*> get_all(Devicegraph* devicegraph);
	static std::vector<const LvmPv*> get_all(const Devicegraph* devicegraph);

	static LvmPv* find_by_uuid(Devicegraph* devicegraph, const std::string& uuid);
	static const LvmPv* find_by_uuid(const Devicegraph* devicegraph, const std::string& uuid);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmPv* clone() const override;

    protected:

	LvmPv(Impl* impl);

    };


    bool is_lvm_pv(const Device* device);

    LvmPv* to_lvm_pv(Device* device);
    const LvmPv* to_lvm_pv(const Device* device);

}

#endif
