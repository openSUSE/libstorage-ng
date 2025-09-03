/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2017-2023] SUSE LLC
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
#include "storage/Filesystems/BtrfsQgroupImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    string
    get_btrfs_raid_level_name(BtrfsRaidLevel btrfs_raid_level)
    {
	return toString(btrfs_raid_level);
    }


    BtrfsSubvolumeNotFoundByPath::BtrfsSubvolumeNotFoundByPath(const string& path)
	: DeviceNotFound(sformat("btrfs subvolume not found, path:%s", path))
    {
    }


    BtrfsQgroupNotFoundById::BtrfsQgroupNotFoundById(const BtrfsQgroup::id_t& id)
	: DeviceNotFound(sformat("btrfs qgroup not found, id:%s", BtrfsQgroup::Impl::format_id(id)))
    {
    }


    Btrfs*
    Btrfs::create(Devicegraph* devicegraph)
    {
	shared_ptr<Btrfs> btrfs = make_shared<Btrfs>(make_unique<Btrfs::Impl>());
	Device::Impl::create(devicegraph, btrfs);

	// create BtrfsSubvolume for the top-level subvolume
	BtrfsSubvolume* top_level = BtrfsSubvolume::create(devicegraph, "");
	Subdevice::create(devicegraph, btrfs.get(), top_level);
	top_level->get_impl().set_id(BtrfsSubvolume::Impl::top_level_id);
	top_level->set_default_btrfs_subvolume();

	return btrfs.get();
    }


    Btrfs*
    Btrfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Btrfs> btrfs = make_shared<Btrfs>(make_unique<Btrfs::Impl>(node));
	Device::Impl::load(devicegraph, btrfs);
	return btrfs.get();
    }


    Btrfs::Btrfs(Impl* impl)
	: BlkFilesystem(impl)
    {
    }


    Btrfs::Btrfs(unique_ptr<Device::Impl>&& impl)
	: BlkFilesystem(std::move(impl))
    {
    }


    BtrfsRaidLevel
    Btrfs::get_metadata_raid_level() const
    {
	return get_impl().get_metadata_raid_level();
    }


    void
    Btrfs::set_metadata_raid_level(BtrfsRaidLevel metadata_raid_level)
    {
	get_impl().set_metadata_raid_level(metadata_raid_level);
    }


    BtrfsRaidLevel
    Btrfs::get_data_raid_level() const
    {
	return get_impl().get_data_raid_level();
    }


    void
    Btrfs::set_data_raid_level(BtrfsRaidLevel data_raid_level)
    {
	get_impl().set_data_raid_level(data_raid_level);
    }


    vector<BtrfsRaidLevel>
    Btrfs::get_allowed_metadata_raid_levels() const
    {
	return get_impl().get_allowed_metadata_raid_levels();
    }


    vector<BtrfsRaidLevel>
    Btrfs::get_allowed_data_raid_levels() const
    {
	return get_impl().get_allowed_data_raid_levels();
    }


    bool
    Btrfs::has_quota() const
    {
	return get_impl().has_quota();
    }


    void
    Btrfs::set_quota(bool quota)
    {
	get_impl().set_quota(quota);
    }


    FilesystemUser*
    Btrfs::add_device(BlkDevice* blk_device)
    {
	return get_impl().add_device(blk_device);
    }


    void
    Btrfs::remove_device(BlkDevice* blk_device)
    {
	return get_impl().remove_device(blk_device);
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


    BtrfsQgroup*
    Btrfs::create_btrfs_qgroup(const BtrfsQgroup::id_t& id)
    {
	return get_impl().create_btrfs_qgroup(id);
    }


    vector<BtrfsQgroup*>
    Btrfs::get_btrfs_qgroups()
    {
	return get_impl().get_btrfs_qgroups();
    }


    vector<const BtrfsQgroup*>
    Btrfs::get_btrfs_qgroups() const
    {
	return get_impl().get_btrfs_qgroups();
    }


    BtrfsQgroup*
    Btrfs::find_btrfs_qgroup_by_id(const BtrfsQgroup::id_t& id)
    {
	return get_impl().find_btrfs_qgroup_by_id(id);
    }


    const BtrfsQgroup*
    Btrfs::find_btrfs_qgroup_by_id(const BtrfsQgroup::id_t& id) const
    {
	return get_impl().find_btrfs_qgroup_by_id(id);
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


    std::unique_ptr<Device>
    Btrfs::clone_v2() const
    {
	return make_unique<Btrfs>(get_impl().clone());
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


    vector<Btrfs*>
    Btrfs::get_all(Devicegraph* devicegraph)
    {
        return devicegraph->get_impl().get_devices_of_type<Btrfs>();
    }


    vector<const Btrfs*>
    Btrfs::get_all(const Devicegraph* devicegraph)
    {
        return devicegraph->get_impl().get_devices_of_type<const Btrfs>();
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
