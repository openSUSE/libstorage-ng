/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#include "storage/Devices/ImplicitPtImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    ImplicitPt*
    ImplicitPt::create(Devicegraph* devicegraph)
    {
	ImplicitPt* ret = new ImplicitPt(new ImplicitPt::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    ImplicitPt*
    ImplicitPt::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	ImplicitPt* ret = new ImplicitPt(new ImplicitPt::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    ImplicitPt::ImplicitPt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    ImplicitPt*
    ImplicitPt::clone() const
    {
	return new ImplicitPt(get_impl().clone());
    }


    ImplicitPt::Impl&
    ImplicitPt::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const ImplicitPt::Impl&
    ImplicitPt::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_implicit_pt(const Device* device)
    {
	return is_device_of_type<const ImplicitPt>(device);
    }


    ImplicitPt*
    to_implicit_pt(Device* device)
    {
	return to_device_of_type<ImplicitPt>(device);
    }


    const ImplicitPt*
    to_implicit_pt(const Device* device)
    {
	return to_device_of_type<const ImplicitPt>(device);
    }

}
