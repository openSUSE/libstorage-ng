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
#include "storage/Utils/StorageTmpl.h"
#include "storage/Devices/MdImpl.h"


namespace storage
{

    const char* HolderTraits<MdSubdevice>::classname = "MdSubdevice";


    MdSubdevice::Impl::Impl(const xmlNode* node)
	: Subdevice::Impl(node), member()
    {
	getChildValue(node, "member", member);
    }


    void
    MdSubdevice::Impl::save(xmlNode* node) const
    {
	Subdevice::Impl::save(node);

	setChildValue(node, "member", member);
    }


    bool
    MdSubdevice::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Subdevice::Impl::equal(rhs))
	    return false;

	return member == rhs.member;
    }


    void
    MdSubdevice::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Subdevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "member", member, rhs.member);
    }


    void
    MdSubdevice::Impl::print(std::ostream& out) const
    {
	Subdevice::Impl::print(out);

	if (!member.empty())
	    out << " member:" << member;
    }

}
