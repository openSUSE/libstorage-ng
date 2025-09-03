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


#include "storage/Devices/MsdosImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Msdos*
    Msdos::create(Devicegraph* devicegraph)
    {
	shared_ptr<Msdos> msdos = make_shared<Msdos>(make_unique<Msdos::Impl>());
	Device::Impl::create(devicegraph, msdos);
	return msdos.get();
    }


    Msdos*
    Msdos::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Msdos> msdos = make_shared<Msdos>(make_unique<Msdos::Impl>(node));
	Device::Impl::load(devicegraph, msdos);
	return msdos.get();
    }


    Msdos::Msdos(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Msdos::Msdos(unique_ptr<Device::Impl>&& impl)
	: PartitionTable(std::move(impl))
    {
    }


    Msdos*
    Msdos::clone() const
    {
	return new Msdos(get_impl().clone());
    }


    std::unique_ptr<Device>
    Msdos::clone_v2() const
    {
	return make_unique<Msdos>(get_impl().clone());
    }


    Msdos::Impl&
    Msdos::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Msdos::Impl&
    Msdos::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    unsigned long
    Msdos::get_minimal_mbr_gap() const
    {
	return get_impl().get_minimal_mbr_gap();
    }


    void
    Msdos::set_minimal_mbr_gap(unsigned long minimal_mbr_gap)
    {
	get_impl().set_minimal_mbr_gap(minimal_mbr_gap);
    }


    bool
    is_msdos(const Device* device)
    {
	return is_device_of_type<const Msdos>(device);
    }


    Msdos*
    to_msdos(Device* device)
    {
	return to_device_of_type<Msdos>(device);
    }


    const Msdos*
    to_msdos(const Device* device)
    {
	return to_device_of_type<const Msdos>(device);
    }

}
