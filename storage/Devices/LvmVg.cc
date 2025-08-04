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


#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    LvmVgNotFoundByVgName::LvmVgNotFoundByVgName(const string& vg_name)
	: DeviceNotFound(sformat("LvmVg not found, vg-name:%s", vg_name))
    {
    }


    InvalidExtentSize::InvalidExtentSize(unsigned long long extent_size)
	: Exception(sformat("invalid extent size '%lld'", extent_size))
    {
    }


    InvalidExtentSize::InvalidExtentSize(const string& msg)
	: Exception(msg)
    {
    }


    LvmVg*
    LvmVg::create(Devicegraph* devicegraph, const string& vg_name)
    {
	shared_ptr<LvmVg> lvm_vg = make_shared<LvmVg>(new LvmVg::Impl(vg_name));
	Device::Impl::create(devicegraph, lvm_vg);
	return lvm_vg.get();
    }


    LvmVg*
    LvmVg::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<LvmVg> lvm_vg = make_shared<LvmVg>(new LvmVg::Impl(node));
	Device::Impl::load(devicegraph, lvm_vg);
	return lvm_vg.get();
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


    const Region&
    LvmVg::get_region() const
    {
	return get_impl().get_region();
    }


    unsigned long long
    LvmVg::get_size() const
    {
	return get_impl().get_size();
    }


    std::string
    LvmVg::get_size_string() const
    {
	return get_impl().get_size_text().translated;
    }


    unsigned long long
    LvmVg::get_extent_size() const
    {
	return get_impl().get_extent_size();
    }


    void
    LvmVg::set_extent_size(unsigned long long extent_size)
    {
	get_impl().set_extent_size(extent_size);
    }


    unsigned long long
    LvmVg::number_of_extents() const
    {
	return get_impl().number_of_extents();
    }


    unsigned long long
    LvmVg::number_of_used_extents() const
    {
	return get_impl().number_of_used_extents();
    }


    unsigned long long
    LvmVg::number_of_free_extents() const
    {
	return get_impl().number_of_free_extents();
    }


    bool
    LvmVg::is_overcommitted() const
    {
	return get_impl().is_overcommitted();
    }


    const string&
    LvmVg::get_vg_name() const
    {
	return get_impl().get_vg_name();
    }


    void
    LvmVg::set_vg_name(const string& vg_name)
    {
	get_impl().set_vg_name(vg_name);
    }


    bool
    LvmVg::is_valid_vg_name(const string& vg_name)
    {
	return Impl::is_valid_vg_name(vg_name);
    }


    LvmPv*
    LvmVg::add_lvm_pv(BlkDevice* blk_device)
    {
	return get_impl().add_lvm_pv(blk_device);
    }


    void
    LvmVg::remove_lvm_pv(BlkDevice* blk_device)
    {
	get_impl().remove_lvm_pv(blk_device);
    }


    vector<LvmVg*>
    LvmVg::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<LvmVg>();
    }


    vector<const LvmVg*>
    LvmVg::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const LvmVg>();
    }


    LvmVg*
    LvmVg::find_by_vg_name(Devicegraph* devicegraph, const string& vg_name)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    LvmVg* lvm_vg = dynamic_cast<LvmVg*>(devicegraph->get_impl()[vertex]);
	    if (lvm_vg && lvm_vg->get_vg_name() == vg_name)
		return lvm_vg;
	}

	ST_THROW(LvmVgNotFoundByVgName(vg_name));
    }


    const LvmVg*
    LvmVg::find_by_vg_name(const Devicegraph* devicegraph, const string& vg_name)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const LvmVg* lvm_vg = dynamic_cast<const LvmVg*>(devicegraph->get_impl()[vertex]);
	    if (lvm_vg && lvm_vg->get_vg_name() == vg_name)
		return lvm_vg;
	}

	ST_THROW(LvmVgNotFoundByVgName(vg_name));
    }


    vector<LvmPv*>
    LvmVg::get_lvm_pvs()
    {
	return get_impl().get_lvm_pvs();
    }


    vector<const LvmPv*>
    LvmVg::get_lvm_pvs() const
    {
	return get_impl().get_lvm_pvs();
    }


    vector<LvmLv*>
    LvmVg::get_lvm_lvs()
    {
	return get_impl().get_lvm_lvs();
    }


    vector<const LvmLv*>
    LvmVg::get_lvm_lvs() const
    {
	return get_impl().get_lvm_lvs();
    }


    unsigned long long
    LvmVg::max_size_for_lvm_lv(LvType lv_type) const
    {
	return get_impl().max_size_for_lvm_lv(lv_type);
    }


    LvmLv*
    LvmVg::create_lvm_lv(const string& lv_name, LvType lv_type, unsigned long long size)
    {
	return get_impl().create_lvm_lv(lv_name, lv_type, size);
    }


    LvmLv*
    LvmVg::create_lvm_lv(const string& lv_name, unsigned long long size)
    {
	return get_impl().create_lvm_lv(lv_name, LvType::NORMAL, size);
    }


    void
    LvmVg::delete_lvm_lv(LvmLv* lvm_lv)
    {
	return get_impl().delete_lvm_lv(lvm_lv);
    }


    LvmLv*
    LvmVg::get_lvm_lv(const string& lv_name)
    {
	return get_impl().get_lvm_lv(lv_name);
    }


    bool
    LvmVg::compare_by_vg_name(const LvmVg* lhs, const LvmVg* rhs)
    {
	return lhs->get_vg_name() < rhs->get_vg_name();
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
