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


#include "storage/Holders/UserImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    User*
    User::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	shared_ptr<User> user = make_shared<User>(make_unique<User::Impl>());
	Holder::Impl::create(devicegraph, source, target, user);
	return user.get();
    }


    User*
    User::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<User> user = make_shared<User>(make_unique<User::Impl>(node));
	Holder::Impl::load(devicegraph, node, user);
	return user.get();
    }


    User::User(Impl* impl)
	: Holder(impl)
    {
    }


    User::User(unique_ptr<Holder::Impl>&& impl)
	: Holder(std::move(impl))
    {
    }


    User*
    User::clone() const
    {
	return new User(get_impl().clone());
    }


    std::unique_ptr<Holder>
    User::clone_v2() const
    {
	return make_unique<User>(get_impl().clone());
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
