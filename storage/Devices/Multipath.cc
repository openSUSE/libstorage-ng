/*
 * Copyright (c) [2017-2023] SUSE LLC
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


#include "storage/Devices/MultipathImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Multipath*
    Multipath::create(Devicegraph* devicegraph, const string& name)
    {
	shared_ptr<Multipath> multipath = make_shared<Multipath>(make_unique<Multipath::Impl>(name));
	Device::Impl::create(devicegraph, multipath);
	return multipath.get();
    }


    Multipath*
    Multipath::create(Devicegraph* devicegraph, const string& name, const Region& region)
    {
	shared_ptr<Multipath> multipath = make_shared<Multipath>(make_unique<Multipath::Impl>(name, region));
	Device::Impl::create(devicegraph, multipath);
	return multipath.get();
    }


    Multipath*
    Multipath::create(Devicegraph* devicegraph, const string& name, unsigned long long size)
    {
	shared_ptr<Multipath> multipath = make_shared<Multipath>(make_unique<Multipath::Impl>(name, Region(0, size / 512, 512)));
	Device::Impl::create(devicegraph, multipath);
	return multipath.get();
    }


    Multipath*
    Multipath::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Multipath> multipath = make_shared<Multipath>(make_unique<Multipath::Impl>(node));
	Device::Impl::load(devicegraph, multipath);
	return multipath.get();
    }


    Multipath::Multipath(Impl* impl)
	: Partitionable(impl)
    {
    }


    Multipath::Multipath(unique_ptr<Device::Impl>&& impl)
	: Partitionable(std::move(impl))
    {
    }


    Multipath*
    Multipath::clone() const
    {
	return new Multipath(get_impl().clone());
    }


    std::unique_ptr<Device>
    Multipath::clone_v2() const
    {
	return make_unique<Multipath>(get_impl().clone());
    }


    Multipath::Impl&
    Multipath::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Multipath::Impl&
    Multipath::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    Multipath*
    Multipath::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_multipath(BlkDevice::find_by_name(devicegraph, name));
    }


    const Multipath*
    Multipath::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_multipath(BlkDevice::find_by_name(devicegraph, name));
    }


    const std::string&
    Multipath::get_vendor() const
    {
	return get_impl().get_vendor();
    }


    const std::string&
    Multipath::get_model() const
    {
	return get_impl().get_model();
    }


    vector<BlkDevice*>
    Multipath::get_blk_devices()
    {
	return get_impl().get_blk_devices();
    }


    vector<const BlkDevice*>
    Multipath::get_blk_devices() const
    {
	return get_impl().get_blk_devices();
    }


    bool
    Multipath::is_rotational() const
    {
	return get_impl().is_rotational();
    }


    vector<Multipath*>
    Multipath::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Multipath>();
    }


    vector<const Multipath*>
    Multipath::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Multipath>();
    }


    bool
    is_multipath(const Device* device)
    {
	return is_device_of_type<const Multipath>(device);
    }


    Multipath*
    to_multipath(Device* device)
    {
	return to_device_of_type<Multipath>(device);
    }


    const Multipath*
    to_multipath(const Device* device)
    {
	return to_device_of_type<const Multipath>(device);
    }

}
