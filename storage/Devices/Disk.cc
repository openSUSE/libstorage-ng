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


#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    string
    get_transport_name(Transport transport)
    {
	return toString(transport);
    }


    string
    get_zone_model_name(ZoneModel zone_model)
    {
	return toString(zone_model);
    }


    Disk*
    Disk::create(Devicegraph* devicegraph, const string& name)
    {
	shared_ptr<Disk> disk = make_shared<Disk>(make_unique<Disk::Impl>(name));
	Device::Impl::create(devicegraph, disk);
	return disk.get();
    }


    Disk*
    Disk::create(Devicegraph* devicegraph, const string& name, const Region& region)
    {
	shared_ptr<Disk> disk = make_shared<Disk>(make_unique<Disk::Impl>(name, region));
	Device::Impl::create(devicegraph, disk);
	return disk.get();
    }


    Disk*
    Disk::create(Devicegraph* devicegraph, const string& name, unsigned long long size)
    {
	shared_ptr<Disk> disk = make_shared<Disk>(make_unique<Disk::Impl>(name, Region(0, size / 512, 512)));
	Device::Impl::create(devicegraph, disk);
	return disk.get();
    }


    Disk*
    Disk::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Disk> disk = make_shared<Disk>(make_unique<Disk::Impl>(node));
	Device::Impl::load(devicegraph, disk);
	return disk.get();
    }


    Disk::Disk(Impl* impl)
	: Partitionable(impl)
    {
    }


    Disk::Disk(unique_ptr<Device::Impl>&& impl)
	: Partitionable(std::move(impl))
    {
    }


    Disk*
    Disk::clone() const
    {
	return new Disk(get_impl().clone());
    }


    std::unique_ptr<Device>
    Disk::clone_v2() const
    {
	return make_unique<Disk>(get_impl().clone());
    }


    Disk::Impl&
    Disk::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Disk::Impl&
    Disk::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    Disk*
    Disk::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_disk(BlkDevice::find_by_name(devicegraph, name));
    }


    const Disk*
    Disk::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_disk(BlkDevice::find_by_name(devicegraph, name));
    }


    bool
    Disk::is_rotational() const
    {
	return get_impl().is_rotational();
    }


    bool
    Disk::is_dax() const
    {
	return get_impl().is_dax();
    }


    Transport
    Disk::get_transport() const
    {
	return get_impl().get_transport();
    }


    ZoneModel
    Disk::get_zone_model() const
    {
	return get_impl().get_zone_model();
    }


    bool
    Disk::is_pmem() const
    {
	return get_impl().is_pmem();
    }


    bool
    Disk::is_nvme() const
    {
	return get_impl().is_nvme();
    }


    const string&
    Disk::get_image_filename() const
    {
	return get_impl().get_image_filename();
    }


    void
    Disk::set_image_filename(const string& image_filename)
    {
	get_impl().set_image_filename(image_filename);
    }


    vector<Disk*>
    Disk::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Disk>();
    }


    vector<const Disk*>
    Disk::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Disk>();
    }


    bool
    is_disk(const Device* device)
    {
	return is_device_of_type<const Disk>(device);
    }


    Disk*
    to_disk(Device* device)
    {
	return to_device_of_type<Disk>(device);
    }


    const Disk*
    to_disk(const Device* device)
    {
	return to_device_of_type<const Disk>(device);
    }

}
