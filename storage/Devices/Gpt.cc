/*
 * Copyright (c) [2014-2015] Novell, Inc.
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
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Gpt*
    Gpt::create(Devicegraph* devicegraph)
    {
	Gpt* ret = new Gpt(new Gpt::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Gpt*
    Gpt::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Gpt* ret = new Gpt(new Gpt::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Gpt::Gpt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Gpt*
    Gpt::clone() const
    {
	return new Gpt(get_impl().clone());
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


    bool
    Gpt::get_enlarge() const
    {
	return get_impl().get_enlarge();
    }


    void
    Gpt::set_enlarge(bool enlarge)
    {
	get_impl().set_enlarge(enlarge);
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
