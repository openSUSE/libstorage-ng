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


#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    string
    get_lv_type_name(LvType lv_type)
    {
	return toString(lv_type);
    }


    LvmLvNotFoundByLvName::LvmLvNotFoundByLvName(const string& lv_name)
	: DeviceNotFound(sformat("LvmLv not found, lv-name:%s", lv_name))
    {
    }


    InvalidStripeSize::InvalidStripeSize(const string& msg)
	: Exception(msg)
    {
    }


    InvalidChunkSize::InvalidChunkSize(const string& msg)
	: Exception(msg)
    {
    }


    LvmLv*
    LvmLv::create(Devicegraph* devicegraph, const string& vg_name, const string& lv_name,
		  LvType lv_type)
    {
	shared_ptr<LvmLv> lvm_lv = make_shared<LvmLv>(new LvmLv::Impl(vg_name, lv_name, lv_type));
	Device::Impl::create(devicegraph, lvm_lv);
	return lvm_lv.get();
    }


    LvmLv*
    LvmLv::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<LvmLv> lvm_lv = make_shared<LvmLv>(new LvmLv::Impl(node));
	Device::Impl::load(devicegraph, lvm_lv);
	return lvm_lv.get();
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


    vector<LvmLv*>
    LvmLv::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<LvmLv>();
    }


    vector<const LvmLv*>
    LvmLv::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const LvmLv>();
    }


    const string&
    LvmLv::get_lv_name() const
    {
	return get_impl().get_lv_name();
    }


    void
    LvmLv::set_lv_name(const string& lv_name)
    {
	get_impl().set_lv_name(lv_name);
    }


    bool
    LvmLv::is_valid_lv_name(const string& lv_name)
    {
	return Impl::is_valid_lv_name(lv_name);
    }


    LvType
    LvmLv::get_lv_type() const
    {
	return get_impl().get_lv_type();
    }


    unsigned int
    LvmLv::get_stripes() const
    {
	return get_impl().get_stripes();
    }


    void
    LvmLv::set_stripes(unsigned int stripes)
    {
	get_impl().set_stripes(stripes);
    }


    unsigned long long
    LvmLv::get_stripe_size() const
    {
	return get_impl().get_stripe_size();
    }


    void
    LvmLv::set_stripe_size(unsigned long long stripe_size)
    {
	get_impl().set_stripe_size(stripe_size);
    }


    unsigned long long
    LvmLv::get_chunk_size() const
    {
	return get_impl().get_chunk_size();
    }


    void
    LvmLv::set_chunk_size(unsigned long long chunk_size)
    {
	get_impl().set_chunk_size(chunk_size);
    }


    const LvmVg*
    LvmLv::get_lvm_vg() const
    {
	return get_impl().get_lvm_vg();
    }


    const LvmLv*
    LvmLv::get_thin_pool() const
    {
	return get_impl().get_thin_pool();
    }


    bool
    LvmLv::has_snapshots() const
    {
	return get_impl().has_snapshots();
    }


    vector<LvmLv*>
    LvmLv::get_snapshots()
    {
	return get_impl().get_snapshots();
    }


    vector<const LvmLv*>
    LvmLv::get_snapshots() const
    {
	return get_impl().get_snapshots();
    }


    bool
    LvmLv::has_origin() const
    {
	return get_impl().has_origin();
    }


    LvmLv*
    LvmLv::get_origin()
    {
	return get_impl().get_origin();
    }


    const LvmLv*
    LvmLv::get_origin() const
    {
	return get_impl().get_origin();
    }


    unsigned long long
    LvmLv::max_size_for_lvm_lv(LvType lv_type) const
    {
	return get_impl().max_size_for_lvm_lv(lv_type);
    }


    LvmLv*
    LvmLv::create_lvm_lv(const string& lv_name, LvType lv_type, unsigned long long size)
    {
	return get_impl().create_lvm_lv(lv_name, lv_type, size);
    }


    LvmLv*
    LvmLv::get_lvm_lv(const string& lv_name)
    {
	return get_impl().get_lvm_lv(lv_name);
    }


    vector<LvmLv*>
    LvmLv::get_lvm_lvs()
    {
	return get_impl().get_lvm_lvs();
    }


    vector<const LvmLv*>
    LvmLv::get_lvm_lvs() const
    {
	return get_impl().get_lvm_lvs();
    }


    bool
    LvmLv::compare_by_lv_name(const LvmLv* lhs, const LvmLv* rhs)
    {
	return lhs->get_lv_name() < rhs->get_lv_name();
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
