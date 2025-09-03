/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) 2023 SUSE LLC
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


#include "storage/Filesystems/NtfsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Ntfs*
    Ntfs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Ntfs> ntfs = make_shared<Ntfs>(make_unique<Ntfs::Impl>());
	Device::Impl::create(devicegraph, ntfs);
	return ntfs.get();
    }


    Ntfs*
    Ntfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Ntfs> ntfs = make_shared<Ntfs>(make_unique<Ntfs::Impl>(node));
	Device::Impl::load(devicegraph, ntfs);
	return ntfs.get();
    }


    Ntfs::Ntfs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Ntfs::Ntfs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Ntfs*
    Ntfs::clone() const
    {
	return new Ntfs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Ntfs::clone_v2() const
    {
	return make_unique<Ntfs>(get_impl().clone());
    }


    Ntfs::Impl&
    Ntfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Ntfs::Impl&
    Ntfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<Ntfs*>
    Ntfs::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Ntfs>();
    }


    vector<const Ntfs*>
    Ntfs::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Ntfs>();
    }


    bool
    is_ntfs(const Device* device)
    {
	return is_device_of_type<const Ntfs>(device);
    }


    Ntfs*
    to_ntfs(Device* device)
    {
	return to_device_of_type<Ntfs>(device);
    }


    const Ntfs*
    to_ntfs(const Device* device)
    {
	return to_device_of_type<const Ntfs>(device);
    }

}
