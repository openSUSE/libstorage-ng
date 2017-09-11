/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Utils/Mockup.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Btrfs>::classname = "Btrfs";


    Btrfs::Impl::Impl()
        : BlkFilesystem::Impl()
        , configure_snapper(false)
        , snapper_config(nullptr)
    {
    }


    Btrfs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
        , configure_snapper(false)
        , snapper_config(nullptr)
    {
    }


    Btrfs::Impl::~Impl()
    {
        if (snapper_config)
            delete snapper_config;
    }


    void
    Btrfs::Impl::check() const
    {
	BlkFilesystem::Impl::check();

	if (num_children_of_type<const BtrfsSubvolume>() != 1)
	    ST_THROW(Exception("top-level subvolume missing"));
    }


    BtrfsSubvolume*
    Btrfs::Impl::get_top_level_btrfs_subvolume()
    {
	vector<BtrfsSubvolume*> tmp = get_children_of_type<BtrfsSubvolume>();
	if (tmp.size() != 1)
	    ST_THROW(Exception("no top-level subvolume found"));

	return tmp.front();
    }


    const BtrfsSubvolume*
    Btrfs::Impl::get_top_level_btrfs_subvolume() const
    {
	vector<const BtrfsSubvolume*> tmp = get_children_of_type<const BtrfsSubvolume>();
	if (tmp.size() != 1)
	    ST_THROW(Exception("no top-level subvolume found"));

	return tmp.front();
    }


    BtrfsSubvolume*
    Btrfs::Impl::get_default_btrfs_subvolume()
    {
	for (BtrfsSubvolume* btrfs_subvolume : get_btrfs_subvolumes())
	{
	    if (btrfs_subvolume->is_default_btrfs_subvolume())
		return btrfs_subvolume;
	}

	ST_THROW(Exception("no default btrfs subvolume found"));
    }


    const BtrfsSubvolume*
    Btrfs::Impl::get_default_btrfs_subvolume() const
    {
	for (const BtrfsSubvolume* btrfs_subvolume : get_btrfs_subvolumes())
	{
	    if (btrfs_subvolume->is_default_btrfs_subvolume())
		return btrfs_subvolume;
	}

	ST_THROW(Exception("no default btrfs subvolume found"));
    }


    void
    Btrfs::Impl::set_default_btrfs_subvolume(BtrfsSubvolume* btrfs_subvolume) const
    {
	btrfs_subvolume->set_default_btrfs_subvolume();
    }


    vector<BtrfsSubvolume*>
    Btrfs::Impl::get_btrfs_subvolumes()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();

	return devicegraph.filter_devices_of_type<BtrfsSubvolume>(
	    devicegraph.descendants(get_vertex(), false)
	);
    }


    vector<const BtrfsSubvolume*>
    Btrfs::Impl::get_btrfs_subvolumes() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();

	return devicegraph.filter_devices_of_type<const BtrfsSubvolume>(
	    devicegraph.descendants(get_vertex(), false)
	);
    }


    FstabEntry*
    Btrfs::Impl::find_etc_fstab_entry(EtcFstab& etc_fstab, const vector<string>& names) const
    {
	for (FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (!fstab_entry->get_mount_opts().has_subvol())
		return fstab_entry;
	}

	return nullptr;
    }


    const FstabEntry*
    Btrfs::Impl::find_etc_fstab_entry(const EtcFstab& etc_fstab, const vector<string>& names) const
    {
	for (const FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (!fstab_entry->get_mount_opts().has_subvol())
		return fstab_entry;
	}

	return nullptr;
    }


    BtrfsSubvolume*
    Btrfs::Impl::find_btrfs_subvolume_by_path(const string& path)
    {
	for (BtrfsSubvolume* btrfs_subvolume : get_btrfs_subvolumes())
	{
	    if (btrfs_subvolume->get_path() == path)
		return btrfs_subvolume;
	}

	ST_THROW(BtrfsSubvolumeNotFoundByPath(path));
    }


    const BtrfsSubvolume*
    Btrfs::Impl::find_btrfs_subvolume_by_path(const string& path) const
    {
	for (const BtrfsSubvolume* btrfs_subvolume : get_btrfs_subvolumes())
	{
	    if (btrfs_subvolume->get_path() == path)
		return btrfs_subvolume;
	}

	ST_THROW(BtrfsSubvolumeNotFoundByPath(path));
    }


    void
    Btrfs::Impl::probe_pass_2(Prober& prober)
    {
	BlkFilesystem::Impl::probe_pass_2(prober);

	const BlkDevice* blk_device = get_blk_device();

	map<long, BtrfsSubvolume*> subvolumes_by_id;

	BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();
	subvolumes_by_id[top_level->get_id()] = top_level;

	unique_ptr<EnsureMounted> ensure_mounted;
	string mount_point = "/tmp/does-not-matter";
	if (Mockup::get_mode() != Mockup::Mode::PLAYBACK)
	{
	    ensure_mounted.reset(new EnsureMounted(top_level));
	    mount_point = ensure_mounted->get_any_mount_point();
	}

	const CmdBtrfsSubvolumeList& cmd_btrfs_subvolume_list =
	    prober.get_system_info().getCmdBtrfsSubvolumeList(blk_device->get_name(), mount_point);

	// Children can be listed after parents in output of 'btrfs subvolume
	// list ...' so several passes over the list of subvolumes are needed.

	for (const CmdBtrfsSubvolumeList::Entry& subvolume : cmd_btrfs_subvolume_list)
	{
	    BtrfsSubvolume* btrfs_subvolume = BtrfsSubvolume::create(prober.get_probed(), subvolume.path);
	    btrfs_subvolume->get_impl().set_id(subvolume.id);

	    subvolumes_by_id[subvolume.id] = btrfs_subvolume;
	}

	for (const CmdBtrfsSubvolumeList::Entry& subvolume : cmd_btrfs_subvolume_list)
	{
	    const BtrfsSubvolume* child = subvolumes_by_id[subvolume.id];
	    const BtrfsSubvolume* parent = subvolumes_by_id[subvolume.parent_id];
	    Subdevice::create(prober.get_probed(), parent, child);
	}

	for (const CmdBtrfsSubvolumeList::Entry& subvolume : cmd_btrfs_subvolume_list)
	{
	    BtrfsSubvolume* btrfs_subvolume = subvolumes_by_id[subvolume.id];
	    btrfs_subvolume->get_impl().probe_pass_2(prober, mount_point);
	}

	if (subvolumes_by_id.size() > 1)
	{
	    const CmdBtrfsSubvolumeGetDefault& cmd_btrfs_subvolume_get_default =
		prober.get_system_info().getCmdBtrfsSubvolumeGetDefault(blk_device->get_name(), mount_point);
	    subvolumes_by_id[cmd_btrfs_subvolume_get_default.get_id()]->get_impl().set_default_btrfs_subvolume();
	}
    }


    ResizeInfo
    Btrfs::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = BlkFilesystem::Impl::detect_resize_info();

	resize_info.combine(ResizeInfo(true, 256 * MiB, 16 * EiB - 1 * B));

	return resize_info;
    }


    uint64_t
    Btrfs::Impl::used_features() const
    {
        uint64_t features = UF_BTRFS | BlkFilesystem::Impl::used_features();

        if (configure_snapper)
            features |= UF_SNAPSHOTS;

        return features;
    }


    void
    Btrfs::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFSBTRFSBIN " --force " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	blk_device->get_impl().wait_for_device();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create btrfs failed"));

	// TODO uuid is included in mkfs output

	probe_uuid();

        // This would fit better in do_mount(), but that one is a const method
        // which would not allow to set the snapper_config member variable.
        // But we need to give the application a chance to set the
        // configure_snapper variable, so the ctor would not be good choice
        // either. This place is guaranteed to be in the commit phase, so this
        // is the best place for the time being.

        if (configure_snapper && !snapper_config)
            snapper_config = new SnapperConfig(to_btrfs(get_non_impl()));
    }


    void Btrfs::Impl::do_mount(CommitData& commit_data, const MountPoint* mount_point) const
    {
        if (snapper_config)
            snapper_config->pre_mount();

        BlkFilesystem::Impl::do_mount(commit_data, mount_point);

        if (snapper_config)
            snapper_config->post_mount();
    }


    void Btrfs::Impl::do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const
    {
        BlkFilesystem::Impl::do_add_to_etc_fstab(commit_data, mount_point);

        if (snapper_config)
            snapper_config->post_add_to_etc_fstab();
    }


    void
    Btrfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO handle mounted

	string cmd_line = BTRFSBIN " filesystem label " + quote(blk_device->get_name()) + " " +
	    quote(get_label());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label btrfs failed"));
    }

}
