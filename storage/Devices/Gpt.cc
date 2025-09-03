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


#include "storage/Devices/GptImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Gpt*
    Gpt::create(Devicegraph* devicegraph)
    {
	shared_ptr<Gpt> gpt = make_shared<Gpt>(make_unique<Gpt::Impl>());
	Device::Impl::create(devicegraph, gpt);
	return gpt.get();
    }


    Gpt*
    Gpt::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Gpt> gpt = make_shared<Gpt>(make_unique<Gpt::Impl>(node));
	Device::Impl::load(devicegraph, gpt);
	return gpt.get();
    }


    Gpt::Gpt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Gpt::Gpt(unique_ptr<Device::Impl>&& impl)
	: PartitionTable(std::move(impl))
    {
    }


    Gpt*
    Gpt::clone() const
    {
	return new Gpt(get_impl().clone());
    }


    std::unique_ptr<Device>
    Gpt::clone_v2() const
    {
	return make_unique<Gpt>(get_impl().clone());
    }


    Gpt::Impl&
    Gpt::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Gpt::Impl&
    Gpt::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<Gpt*>
    Gpt::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Gpt>();
    }


    vector<const Gpt*>
    Gpt::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Gpt>();
    }


    bool
    Gpt::is_pmbr_boot() const
    {
	return get_impl().is_pmbr_boot();
    }


    void
    Gpt::set_pmbr_boot(bool pmbr_boot)
    {
	get_impl().set_pmbr_boot(pmbr_boot);
    }


    bool
    is_gpt(const Device* device)
    {
	return is_device_of_type<const Gpt>(device);
    }


    Gpt*
    to_gpt(Device* device)
    {
	return to_device_of_type<Gpt>(device);
    }


    const Gpt*
    to_gpt(const Device* device)
    {
	return to_device_of_type<const Gpt>(device);
    }

}
