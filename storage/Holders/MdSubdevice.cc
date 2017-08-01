/*
 * Copyright (c) 2017 SUSE LLC
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


#include "storage/Holders/MdSubdeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    MdSubdevice*
    MdSubdevice::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	MdSubdevice* ret = new MdSubdevice(new MdSubdevice::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    MdSubdevice*
    MdSubdevice::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	MdSubdevice* ret = new MdSubdevice(new MdSubdevice::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    MdSubdevice::MdSubdevice(Impl* impl)
	: Subdevice(impl)
    {
    }


    MdSubdevice*
    MdSubdevice::clone() const
    {
	return new MdSubdevice(get_impl().clone());
    }


    MdSubdevice::Impl&
    MdSubdevice::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const MdSubdevice::Impl&
    MdSubdevice::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    const string&
    MdSubdevice::get_member() const
    {
	return get_impl().get_member();
    }


    void
    MdSubdevice::set_member(const string& member)
    {
	get_impl().set_member(member);
    }


    bool
    is_md_subdevice(const Holder* holder)
    {
	return is_holder_of_type<const MdSubdevice>(holder);
    }


    MdSubdevice*
    to_md_subdevice(Holder* holder)
    {
	return to_holder_of_type<MdSubdevice>(holder);
    }


    const MdSubdevice*
    to_md_subdevice(const Holder* holder)
    {
	return to_holder_of_type<const MdSubdevice>(holder);
    }

}
