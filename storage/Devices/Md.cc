/*
 * Copyright (c) [2016-2025] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Devices/MdImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    using namespace std;


    string
    get_md_level_name(MdLevel md_level)
    {
	return toString(md_level);
    }


    string
    get_md_parity_name(MdParity md_parity)
    {
	return toString(md_parity);
    }


    Md*
    Md::create(Devicegraph* devicegraph, const string& name)
    {
	shared_ptr<Md> md = make_shared<Md>(new Md::Impl(name));
	Device::Impl::create(devicegraph, md);
	return md.get();
    }


    Md*
    Md::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Md> md = make_shared<Md>(new Md::Impl(node));
	Device::Impl::load(devicegraph, md);
	return md.get();
    }


    Md::Md(Impl* impl)
	: Partitionable(impl)
    {
    }


    Md*
    Md::clone() const
    {
	return new Md(get_impl().clone());
    }


    Md::Impl&
    Md::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Md::Impl&
    Md::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    Md::is_valid_name(const std::string& name)
    {
	return Impl::is_valid_name(name);
    }


    MdUser*
    Md::add_device(BlkDevice* blk_device)
    {
	return get_impl().add_device(blk_device);
    }


    void
    Md::remove_device(BlkDevice* blk_device)
    {
	return get_impl().remove_device(blk_device);
    }


    vector<BlkDevice*>
    Md::get_devices()
    {
	return get_impl().get_blk_devices();
    }


    vector<const BlkDevice*>
    Md::get_devices() const
    {
	return get_impl().get_blk_devices();
    }


    vector<BlkDevice*>
    Md::get_blk_devices()
    {
	return get_impl().get_blk_devices();
    }


    vector<const BlkDevice*>
    Md::get_blk_devices() const
    {
	return get_impl().get_blk_devices();
    }


    bool
    Md::is_numeric() const
    {
	return get_impl().is_numeric();
    }


    unsigned int
    Md::get_number() const
    {
	return get_impl().get_number();
    }


    Md*
    Md::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_md(BlkDevice::find_by_name(devicegraph, name));
    }


    const Md*
    Md::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_md(BlkDevice::find_by_name(devicegraph, name));
    }


    MdLevel
    Md::get_md_level() const
    {
	return get_impl().get_md_level();
    }


    void
    Md::set_md_level(MdLevel md_level)
    {
	get_impl().set_md_level(md_level);
    }


    MdParity
    Md::get_md_parity() const
    {
	return get_impl().get_md_parity();
    }

    void
    Md::set_md_parity(MdParity md_parity)
    {
	get_impl().set_md_parity(md_parity);
    }


    vector<MdParity>
    Md::get_allowed_md_parities() const
    {
	return get_impl().get_allowed_md_parities();
    }


    bool
    Md::is_chunk_size_meaningful() const
    {
	return get_impl().is_chunk_size_meaningful();
    }


    unsigned long
    Md::get_chunk_size() const
    {
	return get_impl().get_chunk_size();
    }


    void
    Md::set_chunk_size(unsigned long chunk_size)
    {
	get_impl().set_chunk_size(chunk_size);
    }


    const string&
    Md::get_uuid() const
    {
	return get_impl().get_uuid();
    }


    void
    Md::set_uuid(const string& uuid)
    {
	get_impl().set_uuid(uuid);
    }


    const string&
    Md::get_metadata() const
    {
	return get_impl().get_metadata();
    }


    void
    Md::set_metadata(const string& metadata)
    {
	get_impl().set_metadata(metadata);
    }


    unsigned int
    Md::minimal_number_of_devices() const
    {
	return get_impl().minimal_number_of_devices();
    }


    bool
    Md::supports_spare_devices() const
    {
	return get_impl().supports_spare_devices();
    }


    bool
    Md::supports_journal_device() const
    {
	return get_impl().supports_journal_device();
    }


    bool
    Md::is_in_etc_mdadm() const
    {
	return get_impl().is_in_etc_mdadm();
    }


    void
    Md::set_in_etc_mdadm(bool in_etc_mdadm)
    {
	get_impl().set_in_etc_mdadm(in_etc_mdadm);
    }


    vector<Md*>
    Md::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Md>();
    }


    vector<const Md*>
    Md::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Md>();
    }


    vector<Md*>
    Md::get_all_if(Devicegraph* devicegraph, std::function<bool(const Md*)> pred)
    {
	return devicegraph->get_impl().get_devices_of_type_if<Md>(pred);
    }


    vector<const Md*>
    Md::get_all_if(const Devicegraph* devicegraph, std::function<bool(const Md*)> pred)
    {
	return devicegraph->get_impl().get_devices_of_type_if<const Md>(pred);
    }


    string
    Md::find_free_numeric_name(const Devicegraph* devicegraph)
    {
	return Md::Impl::find_free_numeric_name(devicegraph);
    }


    unsigned long long
    Md::calculate_underlying_size(MdLevel md_level, unsigned int number_of_devices, unsigned long long size)
    {
	return Md::Impl::calculate_underlying_size(md_level, number_of_devices, size);
    }


    bool
    Md::compare_by_number(const Md* lhs, const Md* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }


    bool
    is_md(const Device* device)
    {
	return is_device_of_type<const Md>(device);
    }


    Md*
    to_md(Device* device)
    {
	return to_device_of_type<Md>(device);
    }


    const Md*
    to_md(const Device* device)
    {
	return to_device_of_type<const Md>(device);
    }

}
