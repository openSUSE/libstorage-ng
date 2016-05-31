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

#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    LvmVg*
    LvmVg::create(Devicegraph* devicegraph, const string& name)
    {
	LvmVg* ret = new LvmVg(new LvmVg::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    LvmVg*
    LvmVg::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	LvmVg* ret = new LvmVg(new LvmVg::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    LvmVg::LvmVg(Impl* impl)
	: Device(impl)
    {
    }


    LvmVg*
    LvmVg::clone() const
    {
	return new LvmVg(get_impl().clone());
    }


    LvmVg::Impl&
    LvmVg::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const LvmVg::Impl&
    LvmVg::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    const string&
    LvmVg::get_name() const
    {
	return get_impl().get_name();
    }


    void
    LvmVg::set_name(const string& name)
    {
	get_impl().set_name(name);
    }


    vector<LvmVg*>
    LvmVg::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<LvmVg>(compare_by_name);
    }


    vector<const LvmVg*>
    LvmVg::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const LvmVg>(compare_by_name);
    }


    void
    LvmVg::check() const
    {
	if (get_name().empty())
	    cerr << "volume group has no name" << endl;
    }


    LvmLv*
    LvmVg::create_lvm_lv(const string& name)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();

	LvmLv* lvm_lv = LvmLv::create(devicegraph, name);
	Subdevice::create(devicegraph, this, lvm_lv);

	return lvm_lv;
    }


    bool
    is_lvm_vg(const Device* device)
    {
	return is_device_of_type<const LvmVg>(device);
    }


    LvmVg*
    to_lvm_vg(Device* device)
    {
	return to_device_of_type<LvmVg>(device);
    }


    const LvmVg*
    to_lvm_vg(const Device* device)
    {
	return to_device_of_type<const LvmVg>(device);
    }

}
