/*
 * Copyright (c) [2016-2019] SUSE LLC
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
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    const char* HolderTraits<FilesystemUser>::classname = "FilesystemUser";


    FilesystemUser::Impl::Impl(const xmlNode* node)
	: User::Impl(node), journal(false), id(0)
    {
	getChildValue(node, "journal", journal);

	getChildValue(node, "id", id);
    }


    void
    FilesystemUser::Impl::save(xmlNode* node) const
    {
	User::Impl::save(node);

	setChildValueIf(node, "journal", journal, journal);

	setChildValueIf(node, "id", id, id);
    }


    bool
    FilesystemUser::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!User::Impl::equal(rhs))
	    return false;

	return journal == rhs.journal && id == rhs.id;
    }


    void
    FilesystemUser::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	User::Impl::log_diff(log, rhs);

	storage::log_diff(log, "journal", journal, rhs.journal);

	storage::log_diff(log, "id", id, rhs.id);
    }


    void
    FilesystemUser::Impl::print(std::ostream& out) const
    {
	User::Impl::print(out);

	if (journal)
	    out << " journal";

	if (id)
	    out << " id:" << id;
    }

}
