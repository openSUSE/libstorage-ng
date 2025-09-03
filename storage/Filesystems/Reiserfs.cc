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


#include "storage/Filesystems/ReiserfsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Reiserfs*
    Reiserfs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Reiserfs> reiserfs = make_shared<Reiserfs>(make_unique<Reiserfs::Impl>());
	Device::Impl::create(devicegraph, reiserfs);
	return reiserfs.get();
    }


    Reiserfs*
    Reiserfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Reiserfs> reiserfs = make_shared<Reiserfs>(make_unique<Reiserfs::Impl>(node));
	Device::Impl::load(devicegraph, reiserfs);
	return reiserfs.get();
    }


    Reiserfs::Reiserfs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Reiserfs::Reiserfs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Reiserfs*
    Reiserfs::clone() const
    {
	return new Reiserfs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Reiserfs::clone_v2() const
    {
	return make_unique<Reiserfs>(get_impl().clone());
    }


    Reiserfs::Impl&
    Reiserfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Reiserfs::Impl&
    Reiserfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_reiserfs(const Device* device)
    {
	return is_device_of_type<const Reiserfs>(device);
    }


    Reiserfs*
    to_reiserfs(Device* device)
    {
	return to_device_of_type<Reiserfs>(device);
    }


    const Reiserfs*
    to_reiserfs(const Device* device)
    {
	return to_device_of_type<const Reiserfs>(device);
    }

}
