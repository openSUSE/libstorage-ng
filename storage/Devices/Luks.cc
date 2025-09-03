/*
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


#include "storage/Devices/LuksImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Luks*
    Luks::create(Devicegraph* devicegraph, const string& dm_table_name)
    {
	shared_ptr<Luks> luks = make_shared<Luks>(make_unique<Luks::Impl>(dm_table_name));
	Device::Impl::create(devicegraph, luks);
	return luks.get();
    }


    Luks*
    Luks::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Luks> luks = make_shared<Luks>(make_unique<Luks::Impl>(node));
	Device::Impl::load(devicegraph, luks);
	return luks.get();
    }


    Luks::Luks(Impl* impl)
	: Encryption(impl)
    {
    }


    Luks::Luks(unique_ptr<Device::Impl>&& impl)
	: Encryption(std::move(impl))
    {
    }


    Luks*
    Luks::clone() const
    {
	return new Luks(get_impl().clone());
    }


    std::unique_ptr<Device>
    Luks::clone_v2() const
    {
	return make_unique<Luks>(get_impl().clone());
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


    void
    Luks::set_uuid(const string& uuid)
    {
	get_impl().set_uuid(uuid);
    }


    const string&
    Luks::get_label() const
    {
	return get_impl().get_label();
    }


    void
    Luks::set_label(const string& label)
    {
	return get_impl().set_label(label);
    }


    const string&
    Luks::get_format_options() const
    {
	return get_impl().get_format_options();
    }


    void
    Luks::set_format_options(const std::string& format_options)
    {
	get_impl().set_format_options(format_options);
    }


    void
    Luks::reset_activation_infos()
    {
	Luks::Impl::reset_activation_infos();
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
