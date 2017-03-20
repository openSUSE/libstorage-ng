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


#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    BtrfsSubvolume*
    BtrfsSubvolume::create(Devicegraph* devicegraph, const string& path)
    {
	BtrfsSubvolume* ret = new BtrfsSubvolume(new BtrfsSubvolume::Impl(path));
	ret->Device::create(devicegraph);
	return ret;
    }


    BtrfsSubvolume*
    BtrfsSubvolume::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	BtrfsSubvolume* ret = new BtrfsSubvolume(new BtrfsSubvolume::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    BtrfsSubvolume::BtrfsSubvolume(Impl* impl)
	: Mountable(impl)
    {
    }


    long
    BtrfsSubvolume::get_id() const
    {
	return get_impl().get_id();
    }


    bool
    BtrfsSubvolume::is_top_level() const
    {
	return get_impl().is_top_level();
    }


    const string&
    BtrfsSubvolume::get_path() const
    {
	return get_impl().get_path();
    }


    bool
    BtrfsSubvolume::is_nocow() const
    {
	return get_impl().is_nocow();
    }


    void
    BtrfsSubvolume::set_nocow(bool nocow)
    {
	get_impl().set_nocow(nocow);
    }


    bool
    BtrfsSubvolume::is_default_btrfs_subvolume() const
    {
	return get_impl().is_default_btrfs_subvolume();
    }


    void
    BtrfsSubvolume::set_default_btrfs_subvolume()
    {
	get_impl().set_default_btrfs_subvolume();
    }


    Btrfs*
    BtrfsSubvolume::get_btrfs()
    {
	return get_impl().get_btrfs();
    }


    const Btrfs*
    BtrfsSubvolume::get_btrfs() const
    {
	return get_impl().get_btrfs();
    }


    BtrfsSubvolume*
    BtrfsSubvolume::get_top_level_btrfs_subvolume()
    {
	return get_impl().get_top_level_btrfs_subvolume();
    }


    const BtrfsSubvolume*
    BtrfsSubvolume::get_top_level_btrfs_subvolume() const
    {
	return get_impl().get_top_level_btrfs_subvolume();
    }


    BtrfsSubvolume*
    BtrfsSubvolume::create_btrfs_subvolume(const string& path)
    {
	return get_impl().create_btrfs_subvolume(path);
    }


    BtrfsSubvolume*
    BtrfsSubvolume::clone() const
    {
	return new BtrfsSubvolume(get_impl().clone());
    }


    BtrfsSubvolume::Impl&
    BtrfsSubvolume::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BtrfsSubvolume::Impl&
    BtrfsSubvolume::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_btrfs_subvolume(const Device* device)
    {
	return is_device_of_type<const BtrfsSubvolume>(device);
    }


    BtrfsSubvolume*
    to_btrfs_subvolume(Device* device)
    {
	return to_device_of_type<BtrfsSubvolume>(device);
    }


    const BtrfsSubvolume*
    to_btrfs_subvolume(const Device* device)
    {
	return to_device_of_type<const BtrfsSubvolume>(device);
    }

}
