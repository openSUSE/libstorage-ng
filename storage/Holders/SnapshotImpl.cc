/*
 * Copyright (c) 2020 SUSE LLC
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


#include "storage/Holders/SnapshotImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    const char* HolderTraits<Snapshot>::classname = "Snapshot";


    Snapshot::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    Snapshot::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }


    bool
    Snapshot::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	return Holder::Impl::equal(rhs);
    }


    void
    Snapshot::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Holder::Impl::log_diff(log, rhs);
    }


    void
    Snapshot::Impl::print(std::ostream& out) const
    {
	Holder::Impl::print(out);
    }

}
