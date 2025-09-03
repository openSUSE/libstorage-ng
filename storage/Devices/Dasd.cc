/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/DasdImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    string
    get_dasd_type_name(DasdType dasd_type)
    {
	return toString(dasd_type);
    }


    string
    get_dasd_format_name(DasdFormat dasd_format)
    {
	return toString(dasd_format);
    }


    Dasd*
    Dasd::create(Devicegraph* devicegraph, const string& name)
    {
	shared_ptr<Dasd> dasd = make_shared<Dasd>(make_unique<Dasd::Impl>(name));
	Device::Impl::create(devicegraph, dasd);
	return dasd.get();
    }


    Dasd*
    Dasd::create(Devicegraph* devicegraph, const string& name, const Region& region)
    {
	shared_ptr<Dasd> dasd = make_shared<Dasd>(make_unique<Dasd::Impl>(name, region));
	Device::Impl::create(devicegraph, dasd);
	return dasd.get();
    }


    Dasd*
    Dasd::create(Devicegraph* devicegraph, const string& name, unsigned long long size)
    {
	shared_ptr<Dasd> dasd = make_shared<Dasd>(make_unique<Dasd::Impl>(name, Region(0, size / 512, 512)));
	Device::Impl::create(devicegraph, dasd);
	return dasd.get();
    }


    Dasd*
    Dasd::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Dasd> dasd = make_shared<Dasd>(make_unique<Dasd::Impl>(node));
	Device::Impl::load(devicegraph, dasd);
	return dasd.get();
    }


    Dasd::Dasd(Impl* impl)
	: Partitionable(impl)
    {
    }


    Dasd::Dasd(unique_ptr<Device::Impl>&& impl)
	: Partitionable(std::move(impl))
    {
    }


    Dasd*
    Dasd::clone() const
    {
	return new Dasd(get_impl().clone());
    }


    std::unique_ptr<Device>
    Dasd::clone_v2() const
    {
	return make_unique<Dasd>(get_impl().clone());
    }


    Dasd::Impl&
    Dasd::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Dasd::Impl&
    Dasd::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    Dasd*
    Dasd::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_dasd(BlkDevice::find_by_name(devicegraph, name));
    }


    const Dasd*
    Dasd::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_dasd(BlkDevice::find_by_name(devicegraph, name));
    }


    string
    Dasd::get_bus_id() const
    {
	return get_impl().get_bus_id();
    }


    void
    Dasd::set_bus_id(string bus_id)
    {
	get_impl().set_bus_id(bus_id);
    }


    bool
    Dasd::is_rotational() const
    {
	return get_impl().is_rotational();
    }


    DasdType
    Dasd::get_type() const
    {
	return get_impl().get_type();
    }


    void
    Dasd::set_type(DasdType type)
    {
	get_impl().set_type(type);
    }


    DasdFormat
    Dasd::get_format() const
    {
	return get_impl().get_format();
    }


    void
    Dasd::set_format(DasdFormat format)
    {
	get_impl().set_format(format);
    }


    vector<Dasd*>
    Dasd::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Dasd>();
    }


    vector<const Dasd*>
    Dasd::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Dasd>();
    }


    bool
    is_dasd(const Device* device)
    {
	return is_device_of_type<const Dasd>(device);
    }


    Dasd*
    to_dasd(Device* device)
    {
	return to_device_of_type<Dasd>(device);
    }


    const Dasd*
    to_dasd(const Device* device)
    {
	return to_device_of_type<const Dasd>(device);
    }

}
