/*
 * Copyright (c) [2018-2023] SUSE LLC
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


#include "storage/Filesystems/ExfatImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Exfat*
    Exfat::create(Devicegraph* devicegraph)
    {
	shared_ptr<Exfat> exfat = make_shared<Exfat>(make_unique<Exfat::Impl>());
	Device::Impl::create(devicegraph, exfat);
	return exfat.get();
    }


    Exfat*
    Exfat::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Exfat> exfat = make_shared<Exfat>(make_unique<Exfat::Impl>(node));
	Device::Impl::load(devicegraph, exfat);
	return exfat.get();
    }


    Exfat::Exfat(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Exfat::Exfat(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Exfat*
    Exfat::clone() const
    {
	return new Exfat(get_impl().clone());
    }


    std::unique_ptr<Device>
    Exfat::clone_v2() const
    {
	return make_unique<Exfat>(get_impl().clone());
    }


    Exfat::Impl&
    Exfat::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Exfat::Impl&
    Exfat::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_exfat(const Device* device)
    {
	return is_device_of_type<const Exfat>(device);
    }


    Exfat*
    to_exfat(Device* device)
    {
	return to_device_of_type<Exfat>(device);
    }


    const Exfat*
    to_exfat(const Device* device)
    {
	return to_device_of_type<const Exfat>(device);
    }

}
