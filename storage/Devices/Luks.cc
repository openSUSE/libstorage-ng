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


#include "storage/Devices/LuksImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Luks*
    Luks::create(Devicegraph* devicegraph, const string& name)
    {
	Luks* ret = new Luks(new Luks::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    Luks*
    Luks::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Luks* ret = new Luks(new Luks::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Luks::Luks(Impl* impl)
	: Encryption(impl)
    {
    }


    Luks*
    Luks::clone() const
    {
	return new Luks(get_impl().clone());
    }


    Luks::Impl&
    Luks::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Luks::Impl&
    Luks::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<Luks*>
    Luks::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Luks>();
    }


    vector<const Luks*>
    Luks::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Luks>();
    }


    const string&
    Luks::get_uuid() const
    {
	return get_impl().get_uuid();
    }

    bool
    is_luks(const Device* device)
    {
	return is_device_of_type<const Luks>(device);
    }


    Luks*
    to_luks(Device* device)
    {
	return to_device_of_type<Luks>(device);
    }


    const Luks*
    to_luks(const Device* device)
    {
	return to_device_of_type<const Luks>(device);
    }

}
