/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Holders/UserImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    User*
    User::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	User* ret = new User(new User::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    User*
    User::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	User* ret = new User(new User::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    User::User(Impl* impl)
	: Holder(impl)
    {
    }


    User*
    User::clone() const
    {
	return new User(get_impl().clone());
    }


    User::Impl&
    User::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const User::Impl&
    User::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    is_user(const Holder* holder)
    {
	return is_holder_of_type<const User>(holder);
    }


    User*
    to_user(Holder* holder)
    {
	return to_holder_of_type<User>(holder);
    }


    const User*
    to_user(const Holder* holder)
    {
	return to_holder_of_type<const User>(holder);
    }

}
