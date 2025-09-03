/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Holders/SubdeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Subdevice*
    Subdevice::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	shared_ptr<Subdevice> subdevice = make_shared<Subdevice>(make_unique<Subdevice::Impl>());
	Holder::Impl::create(devicegraph, source, target, subdevice);
	return subdevice.get();
    }


    Subdevice*
    Subdevice::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Subdevice> subdevice = make_shared<Subdevice>(make_unique<Subdevice::Impl>(node));
	Holder::Impl::load(devicegraph, node, subdevice);
	return subdevice.get();
    }


    Subdevice::Subdevice(Impl* impl)
	: Holder(impl)
    {
    }


    Subdevice::Subdevice(unique_ptr<Holder::Impl>&& impl)
	: Holder(std::move(impl))
    {
    }


    Subdevice*
    Subdevice::clone() const
    {
	return new Subdevice(get_impl().clone());
    }


    std::unique_ptr<Holder>
    Subdevice::clone_v2() const
    {
	return make_unique<Subdevice>(get_impl().clone());
    }


    Subdevice::Impl&
    Subdevice::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const Subdevice::Impl&
    Subdevice::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    is_subdevice(const Holder* holder)
    {
	return is_holder_of_type<const Subdevice>(holder);
    }


    Subdevice*
    to_subdevice(Holder* holder)
    {
	return to_holder_of_type<Subdevice>(holder);
    }


    const Subdevice*
    to_subdevice(const Holder* holder)
    {
	return to_holder_of_type<const Subdevice>(holder);
    }

}
