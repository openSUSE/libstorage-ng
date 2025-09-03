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


#include "storage/Filesystems/BtrfsQgroupImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    BtrfsQgroup*
    BtrfsQgroup::create(Devicegraph* devicegraph, const id_t& id)
    {
	shared_ptr<BtrfsQgroup> btrfs_qgroup = make_shared<BtrfsQgroup>(make_unique<BtrfsQgroup::Impl>(id));
	Device::Impl::create(devicegraph, btrfs_qgroup);
	return btrfs_qgroup.get();
    }


    BtrfsQgroup*
    BtrfsQgroup::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<BtrfsQgroup> btrfs_qgroup = make_shared<BtrfsQgroup>(make_unique<BtrfsQgroup::Impl>(node));
	Device::Impl::load(devicegraph, btrfs_qgroup);
	return btrfs_qgroup.get();
    }


    BtrfsQgroup::BtrfsQgroup(Impl* impl)
	: Device(impl)
    {
    }


    BtrfsQgroup::BtrfsQgroup(unique_ptr<Device::Impl>&& impl)
	: Device(std::move(impl))
    {
    }


    Btrfs*
    BtrfsQgroup::get_btrfs()
    {
	return get_impl().get_btrfs();
    }


    const Btrfs*
    BtrfsQgroup::get_btrfs() const
    {
	return get_impl().get_btrfs();
    }


    BtrfsQgroup::id_t
    BtrfsQgroup::get_id() const
    {
	return get_impl().get_id();
    }


    unsigned long long
    BtrfsQgroup::get_referenced() const
    {
	return get_impl().get_referenced();
    }


    unsigned long long
    BtrfsQgroup::get_exclusive() const
    {
	return get_impl().get_exclusive();
    }


    bool
    BtrfsQgroup::has_referenced_limit() const
    {
	return get_impl().get_referenced_limit() != std::nullopt;
    }


    unsigned long long
    BtrfsQgroup::get_referenced_limit() const
    {
	if (!has_referenced_limit())
	    ST_THROW(Exception("qgroup has no referenced limit"));

	return get_impl().get_referenced_limit().value();
    }


    void
    BtrfsQgroup::set_referenced_limit(unsigned long long referenced_limit)
    {
	get_impl().set_referenced_limit(referenced_limit);
    }


    void
    BtrfsQgroup::clear_referenced_limit()
    {
	get_impl().set_referenced_limit(std::nullopt);
    }


    bool
    BtrfsQgroup::has_exclusive_limit() const
    {
	return get_impl().get_exclusive_limit() != std::nullopt;
    }


    unsigned long long
    BtrfsQgroup::get_exclusive_limit() const
    {
	if (!has_exclusive_limit())
	    ST_THROW(Exception("qgroup has no exclusive limit"));

	return get_impl().get_exclusive_limit().value();
    }


    void
    BtrfsQgroup::set_exclusive_limit(unsigned long long exclusive_limit)
    {
	get_impl().set_exclusive_limit(exclusive_limit);
    }


    void
    BtrfsQgroup::clear_exclusive_limit()
    {
	get_impl().set_exclusive_limit(std::nullopt);
    }


    bool
    BtrfsQgroup::is_assigned(const BtrfsQgroup* btrfs_qgroup) const
    {
	return get_impl().is_assigned(btrfs_qgroup);
    }


    void
    BtrfsQgroup::assign(BtrfsQgroup* btrfs_qgroup)
    {
	get_impl().assign(btrfs_qgroup);
    }


    void
    BtrfsQgroup::unassign(BtrfsQgroup* btrfs_qgroup)
    {
	get_impl().unassign(btrfs_qgroup);
    }


    vector<BtrfsQgroup*>
    BtrfsQgroup::get_assigned_btrfs_qgroups()
    {
	return get_impl().get_assigned_btrfs_qgroups();
    }


    vector<const BtrfsQgroup*>
    BtrfsQgroup::get_assigned_btrfs_qgroups() const
    {
	return get_impl().get_assigned_btrfs_qgroups();
    }


    BtrfsQgroup*
    BtrfsQgroup::clone() const
    {
	return new BtrfsQgroup(get_impl().clone());
    }


    std::unique_ptr<Device>
    BtrfsQgroup::clone_v2() const
    {
	return make_unique<BtrfsQgroup>(get_impl().clone());
    }


    BtrfsQgroup::Impl&
    BtrfsQgroup::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BtrfsQgroup::Impl&
    BtrfsQgroup::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_btrfs_qgroup(const Device* device)
    {
	return is_device_of_type<const BtrfsQgroup>(device);
    }


    BtrfsQgroup*
    to_btrfs_qgroup(Device* device)
    {
	return to_device_of_type<BtrfsQgroup>(device);
    }


    const BtrfsQgroup*
    to_btrfs_qgroup(const Device* device)
    {
	return to_device_of_type<const BtrfsQgroup>(device);
    }

}
