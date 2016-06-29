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

#include "storage/Devices/MdImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/Enum.h"


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
	if (!Md::Impl::is_valid_name(name))
	    ST_THROW(Exception("invalid md name"));

	Md* ret = new Md(new Md::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    Md*
    Md::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Md* ret = new Md(new Md::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    void
    Md::check() const
    {
	Partitionable::check();

	if (get_region().get_start() != 0)
	    cerr << "md region start not zero" << endl;

	if (!Md::Impl::is_valid_name(get_name()))
	    ST_THROW(Exception("invalid name"));
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
	return get_impl().get_devices();
    }


    vector<const BlkDevice*>
    Md::get_devices() const
    {
	return get_impl().get_devices();
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


    vector<Md*>
    Md::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Md>(compare_by_number);
    }


    vector<const Md*>
    Md::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Md>(compare_by_number);
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
