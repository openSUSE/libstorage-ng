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


#include "storage/Holders/BtrfsQgroupRelationImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/BtrfsQgroupImpl.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Format.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    const char* HolderTraits<BtrfsQgroupRelation>::classname = "BtrfsQgroupRelation";


    BtrfsQgroupRelation::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    BtrfsQgroupRelation::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }


    bool
    BtrfsQgroupRelation::Impl::is_in_view(View view) const
    {
	switch (view)
	{
	    case View::ALL:
		return true;

	    case View::CLASSIC:
		return false;

	    case View::REMOVE:
	    {
		const Device* device = get_source();
		return is_btrfs(device);
	    }
	}

	ST_THROW(LogicException("invalid value for view"));
    }


    const Btrfs*
    BtrfsQgroupRelation::Impl::get_btrfs() const
    {
	return to_btrfs_qgroup(get_target())->get_btrfs();
    }


    bool
    BtrfsQgroupRelation::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	return Holder::Impl::equal(rhs);
    }


    void
    BtrfsQgroupRelation::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Holder::Impl::log_diff(log, rhs);
    }


    void
    BtrfsQgroupRelation::Impl::print(std::ostream& out) const
    {
	Holder::Impl::print(out);
    }

}
