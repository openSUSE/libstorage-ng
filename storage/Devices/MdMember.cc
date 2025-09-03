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


#include "storage/Devices/MdMemberImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    MdMember*
    MdMember::create(Devicegraph* devicegraph, const string& name)
    {
	shared_ptr<MdMember> md_member = make_shared<MdMember>(make_unique<MdMember::Impl>(name));
	Device::Impl::create(devicegraph, md_member);
	return md_member.get();
    }


    MdMember*
    MdMember::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<MdMember> md_member = make_shared<MdMember>(make_unique<MdMember::Impl>(node));
	Device::Impl::load(devicegraph, md_member);
	return md_member.get();
    }


    MdMember::MdMember(Impl* impl)
	: Md(impl)
    {
    }


    MdMember::MdMember(unique_ptr<Device::Impl>&& impl)
	: Md(std::move(impl))
    {
    }


    MdMember*
    MdMember::clone() const
    {
	return new MdMember(get_impl().clone());
    }


    std::unique_ptr<Device>
    MdMember::clone_v2() const
    {
	return make_unique<MdMember>(get_impl().clone());
    }


    MdMember::Impl&
    MdMember::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const MdMember::Impl&
    MdMember::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    MdContainer*
    MdMember::get_md_container()
    {
	return get_impl().get_md_container();
    }


    const MdContainer*
    MdMember::get_md_container() const
    {
	return get_impl().get_md_container();
    }


    bool
    is_md_member(const Device* device)
    {
	return is_device_of_type<const MdMember>(device);
    }


    MdMember*
    to_md_member(Device* device)
    {
	return to_device_of_type<MdMember>(device);
    }


    const MdMember*
    to_md_member(const Device* device)
    {
	return to_device_of_type<const MdMember>(device);
    }

}
