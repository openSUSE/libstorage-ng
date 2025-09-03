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


#include "storage/Devices/DasdPtImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    DasdPt*
    DasdPt::create(Devicegraph* devicegraph)
    {
	shared_ptr<DasdPt> dasd_pt = make_shared<DasdPt>(make_unique<DasdPt::Impl>());
	Device::Impl::create(devicegraph, dasd_pt);
	return dasd_pt.get();
    }


    DasdPt*
    DasdPt::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<DasdPt> dasd_pt = make_shared<DasdPt>(make_unique<DasdPt::Impl>(node));
	Device::Impl::load(devicegraph, dasd_pt);
	return dasd_pt.get();
    }


    DasdPt::DasdPt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    DasdPt::DasdPt(unique_ptr<Device::Impl>&& impl)
	: PartitionTable(std::move(impl))
    {
    }


    DasdPt*
    DasdPt::clone() const
    {
	return new DasdPt(get_impl().clone());
    }


    std::unique_ptr<Device>
    DasdPt::clone_v2() const
    {
	return make_unique<DasdPt>(get_impl().clone());
    }


    DasdPt::Impl&
    DasdPt::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const DasdPt::Impl&
    DasdPt::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_dasd_pt(const Device* device)
    {
	return is_device_of_type<const DasdPt>(device);
    }


    DasdPt*
    to_dasd_pt(Device* device)
    {
	return to_device_of_type<DasdPt>(device);
    }


    const DasdPt*
    to_dasd_pt(const Device* device)
    {
	return to_device_of_type<const DasdPt>(device);
    }

}
