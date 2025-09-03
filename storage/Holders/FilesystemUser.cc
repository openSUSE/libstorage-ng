/*
 * Copyright (c) [2016-2023] SUSE LLC
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


#include "storage/Holders/FilesystemUserImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    FilesystemUser*
    FilesystemUser::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	shared_ptr<FilesystemUser> filesystem_user = make_shared<FilesystemUser>(make_unique<FilesystemUser::Impl>());
	Holder::Impl::create(devicegraph, source, target, filesystem_user);
	return filesystem_user.get();
    }


    FilesystemUser*
    FilesystemUser::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<FilesystemUser> filesystem_user = make_shared<FilesystemUser>(make_unique<FilesystemUser::Impl>(node));
	Holder::Impl::load(devicegraph, node, filesystem_user);
	return filesystem_user.get();
    }


    FilesystemUser::FilesystemUser(Impl* impl)
	: User(impl)
    {
    }


    FilesystemUser::FilesystemUser(unique_ptr<Holder::Impl>&& impl)
	: User(std::move(impl))
    {
    }


    FilesystemUser*
    FilesystemUser::clone() const
    {
	return new FilesystemUser(get_impl().clone());
    }


    std::unique_ptr<Holder>
    FilesystemUser::clone_v2() const
    {
	return make_unique<FilesystemUser>(get_impl().clone());
    }


    FilesystemUser::Impl&
    FilesystemUser::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const FilesystemUser::Impl&
    FilesystemUser::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    FilesystemUser::is_journal() const
    {
	return get_impl().is_journal();
    }


    void
    FilesystemUser::set_journal(bool journal)
    {
	get_impl().set_journal(journal);
    }


    bool
    is_filesystem_user(const Holder* holder)
    {
	return is_holder_of_type<const FilesystemUser>(holder);
    }


    FilesystemUser*
    to_filesystem_user(Holder* holder)
    {
	return to_holder_of_type<FilesystemUser>(holder);
    }


    const FilesystemUser*
    to_filesystem_user(const Holder* holder)
    {
	return to_holder_of_type<const FilesystemUser>(holder);
    }

}
