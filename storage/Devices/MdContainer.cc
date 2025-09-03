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


#include "storage/Devices/MdContainerImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    MdContainer*
    MdContainer::create(Devicegraph* devicegraph, const string& name)
    {
	shared_ptr<MdContainer> md_container = make_shared<MdContainer>(make_unique<MdContainer::Impl>(name));
	Device::Impl::create(devicegraph, md_container);
	return md_container.get();
    }


    MdContainer*
    MdContainer::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<MdContainer> md_container = make_shared<MdContainer>(make_unique<MdContainer::Impl>(node));
	Device::Impl::load(devicegraph, md_container);
	return md_container.get();
    }


    MdContainer::MdContainer(Impl* impl)
	: Md(impl)
    {
    }


    MdContainer::MdContainer(unique_ptr<Device::Impl>&& impl)
	: Md(std::move(impl))
    {
    }


    MdContainer*
    MdContainer::clone() const
    {
	return new MdContainer(get_impl().clone());
    }


    std::unique_ptr<Device>
    MdContainer::clone_v2() const
    {
	return make_unique<MdContainer>(get_impl().clone());
    }


    MdContainer::Impl&
    MdContainer::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const MdContainer::Impl&
    MdContainer::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<MdMember*>
    MdContainer::get_md_members()
    {
	return get_impl().get_md_members();
    }


    vector<const MdMember*>
    MdContainer::get_md_members() const
    {
	return get_impl().get_md_members();
    }


    bool
    is_md_container(const Device* device)
    {
	return is_device_of_type<const MdContainer>(device);
    }


    MdContainer*
    to_md_container(Device* device)
    {
	return to_device_of_type<MdContainer>(device);
    }


    const MdContainer*
    to_md_container(const Device* device)
    {
	return to_device_of_type<const MdContainer>(device);
    }

}
