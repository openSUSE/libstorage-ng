/*
 * Copyright (c) [2020-2023] SUSE LLC
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

    Snapshot*
    Snapshot::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	shared_ptr<Snapshot> snapshot = make_shared<Snapshot>(make_unique<Snapshot::Impl>());
	Holder::Impl::create(devicegraph, source, target, snapshot);
	return snapshot.get();
    }


    Snapshot*
    Snapshot::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Snapshot> snapshot = make_shared<Snapshot>(make_unique<Snapshot::Impl>(node));
	Holder::Impl::load(devicegraph, node, snapshot);
	return snapshot.get();
    }


    Snapshot::Snapshot(Impl* impl)
	: Holder(impl)
    {
    }


    Snapshot::Snapshot(unique_ptr<Holder::Impl>&& impl)
	: Holder(std::move(impl))
    {
    }


    Snapshot*
    Snapshot::clone() const
    {
	return new Snapshot(get_impl().clone());
    }


    std::unique_ptr<Holder>
    Snapshot::clone_v2() const
    {
	return make_unique<Snapshot>(get_impl().clone());
    }


    Snapshot::Impl&
    Snapshot::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const Snapshot::Impl&
    Snapshot::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    is_snapshot(const Holder* holder)
    {
	return is_holder_of_type<const Snapshot>(holder);
    }


    Snapshot*
    to_snapshot(Holder* holder)
    {
	return to_holder_of_type<Snapshot>(holder);
    }


    const Snapshot*
    to_snapshot(const Holder* holder)
    {
	return to_holder_of_type<const Snapshot>(holder);
    }

}
