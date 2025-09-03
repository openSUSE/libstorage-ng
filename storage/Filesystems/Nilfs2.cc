/*
 * Copyright (c) [2022-2023] SUSE LLC
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


#include "storage/Filesystems/Nilfs2Impl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Nilfs2*
    Nilfs2::create(Devicegraph* devicegraph)
    {
	shared_ptr<Nilfs2> nilfs2 = make_shared<Nilfs2>(make_unique<Nilfs2::Impl>());
	Device::Impl::create(devicegraph, nilfs2);
	return nilfs2.get();
    }


    Nilfs2*
    Nilfs2::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Nilfs2> nilfs2 = make_shared<Nilfs2>(make_unique<Nilfs2::Impl>(node));
	Device::Impl::load(devicegraph, nilfs2);
	return nilfs2.get();
    }


    Nilfs2::Nilfs2(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Nilfs2::Nilfs2(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Nilfs2*
    Nilfs2::clone() const
    {
	return new Nilfs2(get_impl().clone());
    }


    std::unique_ptr<Device>
    Nilfs2::clone_v2() const
    {
	return make_unique<Nilfs2>(get_impl().clone());
    }


    Nilfs2::Impl&
    Nilfs2::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Nilfs2::Impl&
    Nilfs2::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_nilfs2(const Device* device)
    {
	return is_device_of_type<const Nilfs2>(device);
    }


    Nilfs2*
    to_nilfs2(Device* device)
    {
	return to_device_of_type<Nilfs2>(device);
    }


    const Nilfs2*
    to_nilfs2(const Device* device)
    {
	return to_device_of_type<const Nilfs2>(device);
    }

}
