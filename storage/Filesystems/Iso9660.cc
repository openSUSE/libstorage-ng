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


#include "storage/Filesystems/Iso9660Impl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Iso9660*
    Iso9660::create(Devicegraph* devicegraph)
    {
	shared_ptr<Iso9660> iso9660 = make_shared<Iso9660>(make_unique<Iso9660::Impl>());
	Device::Impl::create(devicegraph, iso9660);
	return iso9660.get();
    }


    Iso9660*
    Iso9660::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Iso9660> iso9660 = make_shared<Iso9660>(make_unique<Iso9660::Impl>(node));
	Device::Impl::load(devicegraph, iso9660);
	return iso9660.get();
    }


    Iso9660::Iso9660(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Iso9660::Iso9660(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    Iso9660*
    Iso9660::clone() const
    {
	return new Iso9660(get_impl().clone());
    }


    std::unique_ptr<Device>
    Iso9660::clone_v2() const
    {
	return make_unique<Iso9660>(get_impl().clone());
    }


    Iso9660::Impl&
    Iso9660::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Iso9660::Impl&
    Iso9660::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_iso9660(const Device* device)
    {
	return is_device_of_type<const Iso9660>(device);
    }


    Iso9660*
    to_iso9660(Device* device)
    {
	return to_device_of_type<Iso9660>(device);
    }


    const Iso9660*
    to_iso9660(const Device* device)
    {
	return to_device_of_type<const Iso9660>(device);
    }

}
