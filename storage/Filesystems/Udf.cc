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


#include "storage/Filesystems/UdfImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Udf*
    Udf::create(Devicegraph* devicegraph)
    {
	shared_ptr<Udf> udf = make_shared<Udf>(make_unique<Udf::Impl>());
	Device::Impl::create(devicegraph, udf);
	return udf.get();
    }


    Udf*
    Udf::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Udf> udf = make_shared<Udf>(make_unique<Udf::Impl>(node));
	Device::Impl::load(devicegraph, udf);
	return udf.get();
    }


    Udf::Udf(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Udf::Udf(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Udf*
    Udf::clone() const
    {
	return new Udf(get_impl().clone());
    }


    std::unique_ptr<Device>
    Udf::clone_v2() const
    {
	return make_unique<Udf>(get_impl().clone());
    }


    Udf::Impl&
    Udf::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Udf::Impl&
    Udf::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_udf(const Device* device)
    {
	return is_device_of_type<const Udf>(device);
    }


    Udf*
    to_udf(Device* device)
    {
	return to_device_of_type<Udf>(device);
    }


    const Udf*
    to_udf(const Device* device)
    {
	return to_device_of_type<const Udf>(device);
    }

}
