/*
 * Copyright (c) 2016 SUSE LLC
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
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    const char* HolderTraits<MdUser>::classname = "MdUser";


    MdUser::Impl::Impl(const xmlNode* node)
	: User::Impl(node), spare(false), faulty(false)
    {
	getChildValue(node, "spare", spare);
	getChildValue(node, "faulty", faulty);
    }


    void
    MdUser::Impl::save(xmlNode* node) const
    {
	User::Impl::save(node);

	setChildValueIf(node, "spare", spare, spare);
	setChildValueIf(node, "faulty", faulty, faulty);
    }


    bool
    MdUser::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!User::Impl::equal(rhs))
	    return false;

	return spare == rhs.spare && faulty == rhs.faulty;
    }


    void
    MdUser::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	User::Impl::log_diff(log, rhs);

	storage::log_diff(log, "spare", spare, rhs.spare);
	storage::log_diff(log, "faulty", faulty, rhs.faulty);
    }


    void
    MdUser::Impl::print(std::ostream& out) const
    {
	User::Impl::print(out);

	if (spare)
	    out << " spare";

	if (faulty)
	    out << " faulty";
    }

}
