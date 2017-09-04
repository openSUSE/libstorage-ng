/*
 * Copyright (c) 2015 Novell, Inc.
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


#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Holders/Subdevice.h"


namespace storage
{

    using namespace std;


    BtrfsSubvolumeNotFoundByPath::BtrfsSubvolumeNotFoundByPath(const string& path)
	: DeviceNotFound(sformat("btrfs subvolume not found, path:%s", path.c_str()))
    {
    }


    Btrfs*
    Btrfs::create(Devicegraph* devicegraph)
    {
	Btrfs* ret = new Btrfs(new Btrfs::Impl());
	ret->Device::create(devicegraph);

	// create BtrfsSubvolume for the top-level subvolume
	BtrfsSubvolume* top_level = BtrfsSubvolume::create(devicegraph, "");
	Subdevice::create(devicegraph, ret, top_level);
	top_level->get_impl().set_id(BtrfsSubvolume::Impl::top_level_id);
	top_level->set_default_btrfs_subvolume();

	return ret;
    }


    Btrfs*
    Btrfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Btrfs* ret = new Btrfs(new Btrfs::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Btrfs::Btrfs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    BtrfsSubvolume*
    Btrfs::get_top_level_btrfs_subvolume()
    {
	return get_impl().get_top_level_btrfs_subvolume();
    }


    const BtrfsSubvolume*
    Btrfs::get_top_level_btrfs_subvolume() const
    {
	return get_impl().get_top_level_btrfs_subvolume();
    }


    BtrfsSubvolume*
    Btrfs::get_default_btrfs_subvolume()
    {
        return get_impl().get_default_btrfs_subvolume();
    }


    const BtrfsSubvolume*
    Btrfs::get_default_btrfs_subvolume() const
    {
        return get_impl().get_default_btrfs_subvolume();
    }


    void
    Btrfs::set_default_btrfs_subvolume(BtrfsSubvolume* btrfs_subvolume) const
    {
	get_impl().set_default_btrfs_subvolume(btrfs_subvolume);
    }


    vector<BtrfsSubvolume*>
    Btrfs::get_btrfs_subvolumes()
    {
	return get_impl().get_btrfs_subvolumes();
    }


    vector<const BtrfsSubvolume*>
    Btrfs::get_btrfs_subvolumes() const
    {
	return get_impl().get_btrfs_subvolumes();
    }


    BtrfsSubvolume*
    Btrfs::find_btrfs_subvolume_by_path(const std::string& path)
    {
	return get_impl().find_btrfs_subvolume_by_path(path);
    }


    const BtrfsSubvolume*
    Btrfs::find_btrfs_subvolume_by_path(const std::string& path) const
    {
	return get_impl().find_btrfs_subvolume_by_path(path);
    }


    bool
    Btrfs::get_configure_snapper() const
    {
        return get_impl().get_configure_snapper();
    }


    void
    Btrfs::set_configure_snapper(bool configure)
    {
        get_impl().set_configure_snapper(configure);
    }


    Btrfs*
    Btrfs::clone() const
    {
	return new Btrfs(get_impl().clone());
    }


    Btrfs::Impl&
    Btrfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Btrfs::Impl&
    Btrfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_btrfs(const Device* device)
    {
	return is_device_of_type<const Btrfs>(device);
    }


    Btrfs*
    to_btrfs(Device* device)
    {
	return to_device_of_type<Btrfs>(device);
    }


    const Btrfs*
    to_btrfs(const Device* device)
    {
	return to_device_of_type<const Btrfs>(device);
    }

}
