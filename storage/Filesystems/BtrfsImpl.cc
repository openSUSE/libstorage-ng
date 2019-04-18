/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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


#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/EnvironmentImpl.h"
#include "storage/Storage.h"
#include "storage/Utils/Mockup.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Btrfs>::classname = "Btrfs";


    const vector<string> EnumTraits<BtrfsRaidLevel>::names({
	"UNKNOWN", "DEFAULT", "SINGLE", "DUP", "RAID0", "RAID1", "RAID5", "RAID6", "RAID10"
    });


    Btrfs::Impl::Impl()
	: BlkFilesystem::Impl(), configure_snapper(false), snapper_config(nullptr),
	  metadata_raid_level(BtrfsRaidLevel::DEFAULT), data_raid_level(BtrfsRaidLevel::DEFAULT)
    {
    }


    Btrfs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node), configure_snapper(false), snapper_config(nullptr),
	  metadata_raid_level(BtrfsRaidLevel::UNKNOWN), data_raid_level(BtrfsRaidLevel::UNKNOWN)
    {
	string tmp;

	if (getChildValue(node, "metadata-raid-level", tmp))
	    metadata_raid_level = toValueWithFallback(tmp, BtrfsRaidLevel::UNKNOWN);

	if (getChildValue(node, "data-raid-level", tmp))
	    data_raid_level = toValueWithFallback(tmp, BtrfsRaidLevel::UNKNOWN);
    }


    Btrfs::Impl::~Impl()
    {
        if (snapper_config)
            delete snapper_config;
    }


    void
    Btrfs::Impl::save(xmlNode* node) const
    {
	BlkFilesystem::Impl::save(node);

	setChildValue(node, "metadata-raid-level", toString(metadata_raid_level));
	setChildValue(node, "data-raid-level", toString(data_raid_level));
    }


    string
    Btrfs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Btrfs").translated;
    }


    void
    Btrfs::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	BlkFilesystem::Impl::check(check_callbacks);

	if (num_children_of_type<const BtrfsSubvolume>() != 1)
	    ST_THROW(Exception("top-level subvolume missing"));
    }


    vector<BtrfsRaidLevel>
    Btrfs::Impl::get_allowed_metadata_raid_levels() const
    {
	vector<const BlkDevice*> devices = get_blk_devices();

	// For some number of devies more RAID levels work, e.g. RAID5 with two
	// devices, but are not recommended (warning in mkfs.btrfs output) and
	// are also not mentioned in the btrfs wiki
	// (https://btrfs.wiki.kernel.org/index.php/Using_Btrfs_with_Multiple_Devices).

	switch (devices.size())
	{
	    case 0:
		return { };

	    case 1:
		return { BtrfsRaidLevel::SINGLE, BtrfsRaidLevel::DUP };

	    case 2:
		return { BtrfsRaidLevel::SINGLE, BtrfsRaidLevel::RAID0, BtrfsRaidLevel::RAID1 };

	    case 3:
		return { BtrfsRaidLevel::SINGLE, BtrfsRaidLevel::RAID0, BtrfsRaidLevel::RAID1,
			 BtrfsRaidLevel::RAID5 };

	    default:
		return { BtrfsRaidLevel::SINGLE, BtrfsRaidLevel::RAID0, BtrfsRaidLevel::RAID1,
			 BtrfsRaidLevel::RAID5, BtrfsRaidLevel::RAID6, BtrfsRaidLevel::RAID10 };
	}
    }


    vector<BtrfsRaidLevel>
    Btrfs::Impl::get_allowed_data_raid_levels() const
    {
	return get_allowed_metadata_raid_levels();
    }


    FilesystemUser*
    Btrfs::Impl::add_device(BlkDevice* blk_device)
    {
	ST_CHECK_PTR(blk_device);

	if (blk_device->num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(blk_device->num_children(), 0));

	FilesystemUser* filesystem_user = FilesystemUser::create(get_devicegraph(), blk_device, get_non_impl());

	return filesystem_user;
    }


    void
    Btrfs::Impl::remove_device(BlkDevice* blk_device)
    {
	ST_CHECK_PTR(blk_device);

	FilesystemUser* filesystem_user = to_filesystem_user(get_devicegraph()->find_holder(blk_device->get_sid(),
											    get_sid()));

	get_devicegraph()->remove_holder(filesystem_user);
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


    vector<FstabEntry*>
    Btrfs::Impl::find_etc_fstab_entries(EtcFstab& etc_fstab, const vector<string>& names) const
    {
	vector<FstabEntry*> ret;

	for (FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (!fstab_entry->get_mount_opts().has_subvol())
		ret.push_back(fstab_entry);
	}

	return ret;
    }


    vector<const FstabEntry*>
    Btrfs::Impl::find_etc_fstab_entries(const EtcFstab& etc_fstab, const vector<string>& names) const
    {
	vector<const FstabEntry*> ret;

	for (const FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (!fstab_entry->get_mount_opts().has_subvol())
		ret.push_back(fstab_entry);
	}

	return ret;
    }


    vector<const FstabEntry*>
    Btrfs::Impl::find_proc_mounts_entries(SystemInfo& system_info, const vector<string>& names) const
    {
	// see doc/btrfs.md for default id handling

	long default_id = get_default_btrfs_subvolume()->get_id();

	vector<const FstabEntry*> ret;

	const ProcMounts& proc_mounts = system_info.getProcMounts();

	for (const FstabEntry* mount_entry : proc_mounts.get_by_names(names, system_info))
	{
	    if (!mount_entry->get_mount_opts().has_subvol() ||
		mount_entry->get_mount_opts().has_subvol(default_id))
		ret.push_back(mount_entry);
	}

	return ret;
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


    bool
    Btrfs::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkFilesystem::Impl::equal(rhs))
	    return false;

	return metadata_raid_level == rhs.metadata_raid_level && data_raid_level == rhs.data_raid_level;
    }


    void
    Btrfs::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkFilesystem::Impl::log_diff(log, rhs);

	storage::log_diff_enum(log, "metadata-raid-level", metadata_raid_level, rhs.metadata_raid_level);
	storage::log_diff_enum(log, "data-raid-level", data_raid_level, rhs.data_raid_level);
    }


    void
    Btrfs::Impl::print(std::ostream& out) const
    {
	BlkFilesystem::Impl::print(out);

	out << " metadata-raid-level:" << toString(metadata_raid_level) << " data-raid-level:"
	    << toString(data_raid_level);
    }


    void
    Btrfs::Impl::probe_btrfses(Prober& prober)
    {
	if (!support_btrfs_multiple_devices())
	    return;

	SystemInfo& system_info = prober.get_system_info();
	Devicegraph* system = prober.get_system();

	const CmdBtrfsFilesystemShow& cmd_btrfs_filesystem_show = system_info.getCmdBtrfsFilesystemShow();

	for (const CmdBtrfsFilesystemShow::value_type& detected_btrfs : cmd_btrfs_filesystem_show)
	{
	    try
	    {
		if (detected_btrfs.devices.empty())
		    ST_THROW(Exception("btrfs has no blk devices"));

		vector<BlkDevice*> blk_devices;

		for (const CmdBtrfsFilesystemShow::Device& device : detected_btrfs.devices)
		    blk_devices.push_back(BlkDevice::find_by_any_name(system, device.name));

		BlkFilesystem* blk_filesystem = nullptr;

		for (BlkDevice* blk_device : blk_devices)
		{
		    if (!blk_filesystem)
			blk_filesystem = blk_device->create_blk_filesystem(FsType::BTRFS);
		    else
			FilesystemUser::create(system, blk_device, blk_filesystem);
		}

		if (!blk_filesystem)
		    ST_THROW(Exception("no btrfs created"));

		blk_filesystem->get_impl().probe_pass_2a(prober);
		blk_filesystem->get_impl().probe_pass_2b(prober);
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		error_callback(prober.get_probe_callbacks(), sformat(_("Probing file system with UUID %s failed"),
								     detected_btrfs.uuid, exception));
	    }
	}
    }


    void
    Btrfs::Impl::probe_pass_2a(Prober& prober)
    {
	BlkFilesystem::Impl::probe_pass_2a(prober);

	SystemInfo& system_info = prober.get_system_info();

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
	    system_info.getCmdBtrfsSubvolumeList(blk_device->get_name(), mount_point);

	// Children can be listed after parents in output of 'btrfs subvolume
	// list ...' so several passes over the list of subvolumes are needed.

	for (const CmdBtrfsSubvolumeList::Entry& subvolume : cmd_btrfs_subvolume_list)
	{
	    BtrfsSubvolume* btrfs_subvolume = BtrfsSubvolume::create(prober.get_system(), subvolume.path);
	    btrfs_subvolume->get_impl().set_id(subvolume.id);

	    subvolumes_by_id[subvolume.id] = btrfs_subvolume;
	}

	for (const CmdBtrfsSubvolumeList::Entry& subvolume : cmd_btrfs_subvolume_list)
	{
	    const BtrfsSubvolume* child = subvolumes_by_id[subvolume.id];
	    const BtrfsSubvolume* parent = subvolumes_by_id[subvolume.parent_id];
	    Subdevice::create(prober.get_system(), parent, child);
	}

	for (const CmdBtrfsSubvolumeList::Entry& subvolume : cmd_btrfs_subvolume_list)
	{
	    BtrfsSubvolume* btrfs_subvolume = subvolumes_by_id[subvolume.id];
	    btrfs_subvolume->get_impl().probe_pass_2a(prober, mount_point);
	}

	if (subvolumes_by_id.size() > 1)
	{
	    const CmdBtrfsSubvolumeGetDefault& cmd_btrfs_subvolume_get_default =
		system_info.getCmdBtrfsSubvolumeGetDefault(blk_device->get_name(), mount_point);

	    subvolumes_by_id[cmd_btrfs_subvolume_get_default.get_id()]->get_impl().set_default_btrfs_subvolume();
	}

	const CmdBtrfsFilesystemDf& cmd_btrfs_filesystem_df =
	    system_info.getCmdBtrfsFilesystemDf(blk_device->get_name(), mount_point);

	metadata_raid_level = cmd_btrfs_filesystem_df.get_metadata_raid_level();
	data_raid_level = cmd_btrfs_filesystem_df.get_data_raid_level();
    }


    void
    Btrfs::Impl::probe_pass_2b(Prober& prober)
    {
	BlkFilesystem::Impl::probe_pass_2b(prober);

	BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	unique_ptr<EnsureMounted> ensure_mounted;
	string mount_point = "/tmp/does-not-matter";
	if (Mockup::get_mode() != Mockup::Mode::PLAYBACK)
	{
	    ensure_mounted.reset(new EnsureMounted(top_level));
	    mount_point = ensure_mounted->get_any_mount_point();
	}

	vector<BtrfsSubvolume*> btrfs_subvolumes = get_btrfs_subvolumes();
	sort(btrfs_subvolumes.begin(), btrfs_subvolumes.end(), BtrfsSubvolume::compare_by_id);
	for (BtrfsSubvolume* btrfs_subvolume : btrfs_subvolumes)
	{
	    btrfs_subvolume->get_impl().probe_pass_2b(prober, mount_point);
	}
    }


#if 0
    ResizeInfo
    Btrfs::Impl::detect_resize_info_on_disk() const
    {
	if (!get_devicegraph()->get_impl().is_system() && !get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	// TODO btrfs provides a command to query the min size (btrfs
	// inspect-internal min-dev-size /mount-point) but it does reports
	// wrong values
    }
#endif


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
	string cmd_line = MKFSBTRFSBIN " --force";

	if (metadata_raid_level != BtrfsRaidLevel::DEFAULT)
	    cmd_line += " --metadata=" + toString(metadata_raid_level);

	if (data_raid_level != BtrfsRaidLevel::DEFAULT)
	    cmd_line += " --data=" + toString(data_raid_level);

	if (!get_mkfs_options().empty())
	    cmd_line += " " + get_mkfs_options();

	for (const BlkDevice* blk_device : get_blk_devices())
	    cmd_line += " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

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


    void
    Btrfs::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	// TODO handle multiple devices

	const BlkDevice* blk_device_rhs = to_btrfs(rhs)->get_impl().get_blk_device();

	EnsureMounted ensure_mounted(get_filesystem(), false);

	string cmd_line = BTRFSBIN " filesystem resize";
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " " + to_string(blk_device_rhs->get_size());
	else
	    cmd_line += " max";
	cmd_line += " " + quote(ensure_mounted.get_any_mount_point());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Btrfs::Impl::do_mount(CommitData& commit_data, const CommitOptions& commit_options, MountPoint* mount_point) const
    {
        if (snapper_config)
            snapper_config->pre_mount();

        BlkFilesystem::Impl::do_mount(commit_data, commit_options, mount_point);

        if (snapper_config)
            snapper_config->post_mount();
    }


    void
    Btrfs::Impl::do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const
    {
        BlkFilesystem::Impl::do_add_to_etc_fstab(commit_data, mount_point);

        if (snapper_config)
            snapper_config->post_add_to_etc_fstab(commit_data.get_etc_fstab());
    }


    void
    Btrfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO handle mounted

	string cmd_line = BTRFSBIN " filesystem label " + quote(blk_device->get_name()) + " " +
	    quote(get_label());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Btrfs::Impl::do_pre_mount() const
    {
	// Normally using 'btrfs device scan' is not needed but in the
	// inst-sys it seems to be required (see bsc #1096760). Better
	// be save than sorry.

	vector<const BlkDevice*> blk_devices = get_blk_devices();

	if (blk_devices.size() < 2)
	    return;

	string cmd_line = BTRFSBIN " device scan";

	for (const BlkDevice* blk_device : blk_devices)
	    cmd_line += " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::NoThrow);
    }

}
