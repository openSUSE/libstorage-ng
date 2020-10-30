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
#include "storage/Utils/XmlFile.h"


namespace storage
{

    BtrfsQgroupRelation*
    BtrfsQgroupRelation::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	BtrfsQgroupRelation* ret = new BtrfsQgroupRelation(new BtrfsQgroupRelation::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    BtrfsQgroupRelation*
    BtrfsQgroupRelation::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	BtrfsQgroupRelation* ret = new BtrfsQgroupRelation(new BtrfsQgroupRelation::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    BtrfsQgroupRelation::BtrfsQgroupRelation(Impl* impl)
	: Holder(impl)
    {
    }


    BtrfsQgroupRelation*
    BtrfsQgroupRelation::clone() const
    {
	return new BtrfsQgroupRelation(get_impl().clone());
    }


    BtrfsQgroupRelation::Impl&
    BtrfsQgroupRelation::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const BtrfsQgroupRelation::Impl&
    BtrfsQgroupRelation::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    is_btrfs_qgroup_relation(const Holder* holder)
    {
	return is_holder_of_type<const BtrfsQgroupRelation>(holder);
    }


    BtrfsQgroupRelation*
    to_btrfs_qgroup_relation(Holder* holder)
    {
	return to_holder_of_type<BtrfsQgroupRelation>(holder);
    }


    const BtrfsQgroupRelation*
    to_btrfs_qgroup_relation(const Holder* holder)
    {
	return to_holder_of_type<const BtrfsQgroupRelation>(holder);
    }

}
