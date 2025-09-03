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


#include "storage/Filesystems/F2fsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    F2fs*
    F2fs::create(Devicegraph* devicegraph)
    {
	shared_ptr<F2fs> f2fs = make_shared<F2fs>(make_unique<F2fs::Impl>());
	Device::Impl::create(devicegraph, f2fs);
	return f2fs.get();
    }


    F2fs*
    F2fs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<F2fs> f2fs = make_shared<F2fs>(make_unique<F2fs::Impl>(node));
	Device::Impl::load(devicegraph, f2fs);
	return f2fs.get();
    }


    F2fs::F2fs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    F2fs::F2fs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    F2fs*
    F2fs::clone() const
    {
	return new F2fs(get_impl().clone());
    }


    std::unique_ptr<Device>
    F2fs::clone_v2() const
    {
	return make_unique<F2fs>(get_impl().clone());
    }


    F2fs::Impl&
    F2fs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const F2fs::Impl&
    F2fs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_f2fs(const Device* device)
    {
	return is_device_of_type<const F2fs>(device);
    }


    F2fs*
    to_f2fs(Device* device)
    {
	return to_device_of_type<F2fs>(device);
    }


    const F2fs*
    to_f2fs(const Device* device)
    {
	return to_device_of_type<const F2fs>(device);
    }

}
