/*
 * Copyright (c) [2016-2023] SUSE LLC
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


#include "storage/Devices/LvmPvImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    LvmPv*
    LvmPv::create(Devicegraph* devicegraph)
    {
	shared_ptr<LvmPv> lvm_pv = make_shared<LvmPv>(make_unique<LvmPv::Impl>());
	Device::Impl::create(devicegraph, lvm_pv);
	return lvm_pv.get();
    }


    LvmPv*
    LvmPv::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<LvmPv> lvm_pv = make_shared<LvmPv>(make_unique<LvmPv::Impl>(node));
	Device::Impl::load(devicegraph, lvm_pv);
	return lvm_pv.get();
    }


    LvmPv::LvmPv(Impl* impl)
	: Device(impl)
    {
    }


    LvmPv::LvmPv(unique_ptr<Device::Impl>&& impl)
	: Device(std::move(impl))
    {
    }


    LvmPv*
    LvmPv::clone() const
    {
	return new LvmPv(get_impl().clone());
    }


    std::unique_ptr<Device>
    LvmPv::clone_v2() const
    {
	return make_unique<LvmPv>(get_impl().clone());
    }


    LvmPv::Impl&
    LvmPv::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const LvmPv::Impl&
    LvmPv::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<LvmPv*>
    LvmPv::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<LvmPv>();
    }


    vector<const LvmPv*>
    LvmPv::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const LvmPv>();
    }


    bool
    LvmPv::has_blk_device() const
    {
	return get_impl().has_blk_device();
    }


    BlkDevice*
    LvmPv::get_blk_device()
    {
	return get_impl().get_blk_device();
    }


    const BlkDevice*
    LvmPv::get_blk_device() const
    {
	return get_impl().get_blk_device();
    }


    unsigned long long
    LvmPv::get_usable_size() const
    {
	return get_impl().get_usable_size();
    }


    bool
    LvmPv::has_lvm_vg() const
    {
	return get_impl().has_lvm_vg();
    }


    LvmVg*
    LvmPv::get_lvm_vg()
    {
	return get_impl().get_lvm_vg();
    }


    const LvmVg*
    LvmPv::get_lvm_vg() const
    {
	return get_impl().get_lvm_vg();
    }


    bool
    is_lvm_pv(const Device* device)
    {
	return is_device_of_type<const LvmPv>(device);
    }


    LvmPv*
    to_lvm_pv(Device* device)
    {
	return to_device_of_type<LvmPv>(device);
    }


    const LvmPv*
    to_lvm_pv(const Device* device)
    {
	return to_device_of_type<const LvmPv>(device);
    }

}
