/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Filesystems/Ext3Impl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Ext3*
    Ext3::create(Devicegraph* devicegraph)
    {
	shared_ptr<Ext3> ext3 = make_shared<Ext3>(make_unique<Ext3::Impl>());
	Device::Impl::create(devicegraph, ext3);
	return ext3.get();
    }


    Ext3*
    Ext3::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Ext3> ext3 = make_shared<Ext3>(make_unique<Ext3::Impl>(node));
	Device::Impl::load(devicegraph, ext3);
	return ext3.get();
    }


    Ext3::Ext3(Impl* impl)
	: Ext(impl)
    {
    }


    Ext3::Ext3(unique_ptr<Device::Impl>&& impl)
	: Ext(std::move(impl))
    {
    }


    Ext3*
    Ext3::clone() const
    {
	return new Ext3(get_impl().clone());
    }


    std::unique_ptr<Device>
    Ext3::clone_v2() const
    {
	return make_unique<Ext3>(get_impl().clone());
    }


    Ext3::Impl&
    Ext3::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Ext3::Impl&
    Ext3::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_ext3(const Device* device)
    {
	return is_device_of_type<const Ext3>(device);
    }


    Ext3*
    to_ext3(Device* device)
    {
	return to_device_of_type<Ext3>(device);
    }


    const Ext3*
    to_ext3(const Device* device)
    {
	return to_device_of_type<const Ext3>(device);
    }

}
