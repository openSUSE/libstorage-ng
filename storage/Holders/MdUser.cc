/*
 * Copyright (c) [2016-2020] SUSE LLC
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


#include "storage/Holders/MdUserImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    MdUser*
    MdUser::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	MdUser* ret = new MdUser(new MdUser::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    MdUser*
    MdUser::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	MdUser* ret = new MdUser(new MdUser::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    MdUser::MdUser(Impl* impl)
	: User(impl)
    {
    }


    MdUser*
    MdUser::clone() const
    {
	return new MdUser(get_impl().clone());
    }


    MdUser::Impl&
    MdUser::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const MdUser::Impl&
    MdUser::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    MdUser::is_spare() const
    {
	return get_impl().is_spare();
    }


    void
    MdUser::set_spare(bool spare)
    {
	get_impl().set_spare(spare);
    }


    bool
    MdUser::is_faulty() const
    {
	return get_impl().is_faulty();
    }


    void
    MdUser::set_faulty(bool faulty)
    {
	get_impl().set_faulty(faulty);
    }


    bool
    MdUser::is_journal() const
    {
	return get_impl().is_journal();
    }


    void
    MdUser::set_journal(bool journal)
    {
	get_impl().set_journal(journal);
    }


    unsigned int
    MdUser::get_sort_key() const
    {
	return get_impl().get_sort_key();
    }


    void
    MdUser::set_sort_key(unsigned int sort_key)
    {
	get_impl().set_sort_key(sort_key);
    }


    bool
    is_md_user(const Holder* holder)
    {
	return is_holder_of_type<const MdUser>(holder);
    }


    MdUser*
    to_md_user(Holder* holder)
    {
	return to_holder_of_type<MdUser>(holder);
    }


    const MdUser*
    to_md_user(const Holder* holder)
    {
	return to_holder_of_type<const MdUser>(holder);
    }

}
