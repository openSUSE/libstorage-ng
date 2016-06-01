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


#include <iostream>

#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/FindBy.h"


namespace storage
{

    using namespace std;


    LvmLv*
    LvmLv::create(Devicegraph* devicegraph, const string& name)
    {
	LvmLv* ret = new LvmLv(new LvmLv::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    LvmLv*
    LvmLv::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	LvmLv* ret = new LvmLv(new LvmLv::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    LvmLv::LvmLv(Impl* impl)
	: BlkDevice(impl)
    {
    }


    LvmLv*
    LvmLv::clone() const
    {
	return new LvmLv(get_impl().clone());
    }


    LvmLv::Impl&
    LvmLv::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const LvmLv::Impl&
    LvmLv::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    void
    LvmLv::check() const
    {
	if (get_name().empty())
	    cerr << "logical volume has no name" << endl;
    }


    LvmLv*
    LvmLv::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<LvmLv>(devicegraph, uuid);
    }


    const LvmLv*
    LvmLv::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const LvmLv>(devicegraph, uuid);
    }


    bool
    is_lvm_lv(const Device* device)
    {
	return is_device_of_type<const LvmLv>(device);
    }


    LvmLv*
    to_lvm_lv(Device* device)
    {
	return to_device_of_type<LvmLv>(device);
    }


    const LvmLv*
    to_lvm_lv(const Device* device)
    {
	return to_device_of_type<const LvmLv>(device);
    }

}
