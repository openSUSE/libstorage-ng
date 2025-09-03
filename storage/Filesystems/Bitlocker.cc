/*
 * Copyright (c) [2020-2023] SUSE LLC
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


#include "storage/Filesystems/BitlockerImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Bitlocker*
    Bitlocker::create(Devicegraph* devicegraph)
    {
	shared_ptr<Bitlocker> bitlocker = make_shared<Bitlocker>(make_unique<Bitlocker::Impl>());
	Device::Impl::create(devicegraph, bitlocker);
	return bitlocker.get();
    }


    Bitlocker*
    Bitlocker::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Bitlocker> bitlocker = make_shared<Bitlocker>(make_unique<Bitlocker::Impl>(node));
	Device::Impl::load(devicegraph, bitlocker);
	return bitlocker.get();
    }


    Bitlocker::Bitlocker(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Bitlocker::Bitlocker(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Bitlocker*
    Bitlocker::clone() const
    {
	return new Bitlocker(get_impl().clone());
    }


    std::unique_ptr<Device>
    Bitlocker::clone_v2() const
    {
	return make_unique<Bitlocker>(get_impl().clone());
    }


    Bitlocker::Impl&
    Bitlocker::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Bitlocker::Impl&
    Bitlocker::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_bitlocker(const Device* device)
    {
	return is_device_of_type<const Bitlocker>(device);
    }


    Bitlocker*
    to_bitlocker(Device* device)
    {
	return to_device_of_type<Bitlocker>(device);
    }


    const Bitlocker*
    to_bitlocker(const Device* device)
    {
	return to_device_of_type<const Bitlocker>(device);
    }

}
