/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#include "storage/Utils/XmlFile.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/EtcFstab.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/Snapshot.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<BtrfsSubvolume>::classname = "BtrfsSubvolume";


    const long BtrfsSubvolume::Impl::top_level_id;
    const long BtrfsSubvolume::Impl::unknown_id;


    BtrfsSubvolume::Impl::Impl(const xmlNode* node)
	: Mountable::Impl(node), id(unknown_id), path(), default_btrfs_subvolume(false), nocow(false)
    {
	getChildValue(node, "id", id);
	getChildValue(node, "path", path);

	getChildValue(node, "default-btrfs-subvolume", default_btrfs_subvolume);
	getChildValue(node, "nocow", nocow);
    }


    string
    BtrfsSubvolume::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Btrfs Subvolume").translated;
    }


    void
    BtrfsSubvolume::Impl::save(xmlNode* node) const
    {
	Mountable::Impl::save(node);

	setChildValueIf(node, "id", id, id != unknown_id);
	setChildValue(node, "path", path);

	setChildValueIf(node, "default-btrfs-subvolume", default_btrfs_subvolume, default_btrfs_subvolume);
	setChildValueIf(node, "nocow", nocow, nocow);
    }


    string
    BtrfsSubvolume::Impl::get_displayname() const
    {
	return id == top_level_id ? "'top level'" : path;
    }


    void
    BtrfsSubvolume::Impl::set_default_btrfs_subvolume()
    {
	if (!default_btrfs_subvolume)
	{
	    Btrfs* btrfs = get_btrfs();

	    for (BtrfsSubvolume* btrfs_subvolume : btrfs->get_btrfs_subvolumes())
		btrfs_subvolume->get_impl().default_btrfs_subvolume = false;
	}

	default_btrfs_subvolume = true;
    }


    Btrfs*
    BtrfsSubvolume::Impl::get_btrfs()
    {
	BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	return top_level->get_impl().get_single_parent_of_type<Btrfs>();
    }


    const Btrfs*
    BtrfsSubvolume::Impl::get_btrfs() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	return top_level->get_impl().get_single_parent_of_type<const Btrfs>();
    }


    BtrfsSubvolume*
    BtrfsSubvolume::Impl::get_top_level_btrfs_subvolume()
    {
	BtrfsSubvolume* ret = get_non_impl();

	while (true)
	{
	    if (ret->get_id() == top_level_id)
		return ret;

	    ret = ret->get_impl().get_single_parent_of_type<BtrfsSubvolume>();
	}
    }


    const BtrfsSubvolume*
    BtrfsSubvolume::Impl::get_top_level_btrfs_subvolume() const
    {
	const BtrfsSubvolume* ret = get_non_impl();

	while (true)
	{
	    if (ret->get_id() == top_level_id)
		return ret;

	    ret = ret->get_impl().get_single_parent_of_type<const BtrfsSubvolume>();
	}
    }


    Filesystem*
    BtrfsSubvolume::Impl::get_filesystem()
    {
	return get_btrfs();
    }


    const Filesystem*
    BtrfsSubvolume::Impl::get_filesystem() const
    {
	return get_btrfs();
    }


    bool
    BtrfsSubvolume::Impl::has_snapshots() const
    {
	return !get_out_holders_of_type<const Snapshot>(View::ALL).empty();
    }


    vector<BtrfsSubvolume*>
    BtrfsSubvolume::Impl::get_snapshots()
    {
	vector<BtrfsSubvolume*> ret;

	for (Snapshot* snapshot : get_out_holders_of_type<Snapshot>(View::ALL))
	    ret.push_back(to_btrfs_subvolume(snapshot->get_target()));

	return ret;
    }


    vector<const BtrfsSubvolume*>
    BtrfsSubvolume::Impl::get_snapshots() const
    {
	vector<const BtrfsSubvolume*> ret;

	for (const Snapshot* snapshot : get_out_holders_of_type<const Snapshot>(View::ALL))
	    ret.push_back(to_btrfs_subvolume(snapshot->get_target()));

	return ret;
    }


    bool
    BtrfsSubvolume::Impl::has_origin() const
    {
	return !get_in_holders_of_type<const Snapshot>(View::ALL).empty();
    }


    BtrfsSubvolume*
    BtrfsSubvolume::Impl::get_origin()
    {
	vector<Snapshot*> snapshots = get_in_holders_of_type<Snapshot>(View::ALL);

	if (snapshots.size() != 1)
	    ST_THROW(WrongNumberOfParents(snapshots.size(), 1));

	return to_btrfs_subvolume(snapshots.front()->get_source());
    }


    const BtrfsSubvolume*
    BtrfsSubvolume::Impl::get_origin() const
    {
	vector<const Snapshot*> snapshots = get_in_holders_of_type<const Snapshot>(View::ALL);

	if (snapshots.size() != 1)
	    ST_THROW(WrongNumberOfParents(snapshots.size(), 1));

	return to_btrfs_subvolume(snapshots.front()->get_source());
    }


    Text
    BtrfsSubvolume::Impl::get_message_name() const
    {
	return join(get_btrfs()->get_blk_devices(), JoinMode::COMMA, 10);
    }


    string
    BtrfsSubvolume::Impl::get_mount_name() const
    {
	return get_btrfs()->get_impl().get_mount_name();
    }


    string
    BtrfsSubvolume::Impl::get_mount_by_name(const MountPoint* mount_point) const
    {
	return get_btrfs()->get_impl().get_mount_by_name(mount_point);
    }


    MountByType
    BtrfsSubvolume::Impl::get_default_mount_by() const
    {
	return get_btrfs()->get_impl().get_default_mount_by();
    }


    MountOpts
    BtrfsSubvolume::Impl::default_mount_options() const
    {
	// NOTE: This is used for both: fstab and mount command.
	// In case of fstab, subvol path cannot be quoted, and the usage of scape
	// commands like \040 and \011 is undefined (and probably not supported by
	// the tools). Comma might be dangerous due to it is used as separator for
	// mount options.
	return MountOpts(vector<string>({ "subvol=/" + path }));
    }


    vector<string>
    BtrfsSubvolume::Impl::get_mount_options() const
    {
	// See default_mount_options()
	return vector<string>({ "subvol=/" + path });
    }


    vector<ExtendedFstabEntry>
    BtrfsSubvolume::Impl::find_etc_fstab_entries_unfiltered(SystemInfo& system_info) const
    {
	return get_btrfs()->get_impl().find_etc_fstab_entries_unfiltered(system_info);
    }


    vector<FstabEntry*>
    BtrfsSubvolume::Impl::find_etc_fstab_entries_unfiltered(EtcFstab& etc_fstab, const FstabAnchor& fstab_anchor) const
    {
	return get_btrfs()->get_impl().Mountable::Impl::find_etc_fstab_entries_unfiltered(etc_fstab, fstab_anchor);
    }


    bool
    BtrfsSubvolume::Impl::predicate_etc_fstab(const FstabEntry* fstab_entry) const
    {
	return fstab_entry->get_mount_opts().has_subvol(id, path);
    }


    vector<ExtendedFstabEntry>
    BtrfsSubvolume::Impl::find_proc_mounts_entries_unfiltered(SystemInfo& system_info) const
    {
	return get_btrfs()->get_impl().find_proc_mounts_entries_unfiltered(system_info);
    }


    bool
    BtrfsSubvolume::Impl::predicate_proc_mounts(const FstabEntry* fstab_entry) const
    {
	// see doc/btrfs.md for default id handling

	long default_id = get_btrfs()->get_default_btrfs_subvolume()->get_id();

	return fstab_entry->get_mount_opts().has_subvol(id, path) && id != default_id;
    }


    BtrfsSubvolume*
    BtrfsSubvolume::Impl::create_btrfs_subvolume(const string& path)
    {
	// TODO checks, e.g. path?

	Devicegraph* devicegraph = get_devicegraph();

	BtrfsSubvolume* btrfs_subvolume = create(devicegraph, path);

	Subdevice::create(devicegraph, get_non_impl(), btrfs_subvolume);

	return btrfs_subvolume;
    }


    void
    BtrfsSubvolume::Impl::probe_pass_2a(Prober& prober, const string& mount_point)
    {
	SystemInfo& system_info = prober.get_system_info();

	const Btrfs* btrfs = get_btrfs();
	const BlkDevice* blk_device = btrfs->get_impl().get_blk_device();

	const CmdLsattr& cmdlsattr = system_info.getCmdLsattr(blk_device->get_name(), mount_point, path);
	nocow = cmdlsattr.is_nocow();
    }


    void
    BtrfsSubvolume::Impl::probe_pass_2b(Prober& prober, const string& mount_point)
    {
	Mountable::Impl::probe_pass_2b(prober);
    }


    void
    BtrfsSubvolume::Impl::probe_id(const string& mount_point)
    {
	const Btrfs* btrfs = get_btrfs();
	const BlkDevice* blk_device = btrfs->get_impl().get_blk_device();

	const CmdBtrfsSubvolumeList& cmd_btrfs_subvolume_list =
	    CmdBtrfsSubvolumeList(blk_device->get_name(), mount_point);

	CmdBtrfsSubvolumeList::const_iterator it = cmd_btrfs_subvolume_list.find_entry_by_path(path);
	if (it != cmd_btrfs_subvolume_list.end())
	    id = it->id;
    }


    bool
    BtrfsSubvolume::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Mountable::Impl::equal(rhs))
	    return false;

	return id == rhs.id && path == rhs.path && default_btrfs_subvolume == rhs.default_btrfs_subvolume &&
	    nocow == rhs.nocow;
    }


    void
    BtrfsSubvolume::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Mountable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "id", id, rhs.id);
	storage::log_diff(log, "path", path, rhs.path);

	storage::log_diff(log, "default-btrfs-subvolume", default_btrfs_subvolume, rhs.default_btrfs_subvolume);
	storage::log_diff(log, "nocow", nocow, rhs.nocow);
    }


    void
    BtrfsSubvolume::Impl::print(std::ostream& out) const
    {
	Mountable::Impl::print(out);

	out << " id:" << id;
	out << " path:" << path;

	if (default_btrfs_subvolume)
	    out << " default-btrfs-subvolume:" << default_btrfs_subvolume;

	if (nocow)
	    out << " nocow:" << nocow;
    }


    ResizeInfo
    BtrfsSubvolume::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    BtrfsSubvolume::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid(), is_top_level()));

	if (nocow)
	    actions.push_back(new Action::SetNocow(get_sid()));

	if (default_btrfs_subvolume && !is_top_level())
	    actions.push_back(new Action::SetDefaultBtrfsSubvolume(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    BtrfsSubvolume::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Mountable::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_id() != lhs.get_id())
	{
	    ST_THROW(Exception("cannot change btrfs subvolume id"));
	}

	if (nocow != lhs.nocow)
	{
	    Action::Base* action = new Action::SetNocow(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (default_btrfs_subvolume && !lhs.default_btrfs_subvolume)
	{
	    Action::Base* action = new Action::SetDefaultBtrfsSubvolume(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    void
    BtrfsSubvolume::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	// set sync_only if this is the top-level subvolume
	bool sync_only = is_top_level();

	// set nop if the btrfs filesystem is also deleted
	bool nop = !actiongraph.exists_in(get_btrfs(), RHS);

	actions.push_back(new Action::Delete(get_sid(), sync_only, nop));

	actiongraph.add_chain(actions);
    }


    Text
    BtrfsSubvolume::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Create subvolume %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Creating subvolume %1$s on %2$s"));

	return sformat(text, path, get_message_name());
    }


    void
    BtrfsSubvolume::Impl::do_create()
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	// TODO It is not always required to mount to top-level subvolume,
	// e.g. when creating <fs-tree>/a/b it is enough when subvol=a is
	// mounted somewhere.

	// TODO Just do one mount if several subvolumes get created.

	EnsureMounted ensure_mounted(top_level, false);

	string full_path = ensure_mounted.get_any_mount_point() + "/" + path;
	string full_dirname = dirname(full_path);

	if (access(full_dirname.c_str(), R_OK) != 0)
	{
	    createPath(full_dirname);
	}
	else if (access(full_path.c_str(), R_OK) == 0)
	{
	    // TODO rmdir can fail if the directory is not empty. But removing
	    // normal files should not be done.

	    rmdir(full_path.c_str());
	}

	string cmd_line = BTRFS_BIN " subvolume create " + quote(full_path);

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	probe_id(ensure_mounted.get_any_mount_point());
    }


    Text
    BtrfsSubvolume::Impl::do_mount_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the subvolume path (e.g. home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB)),
			   // %3$s is replaced by the mount point (e.g. /home)
			   _("Mount subvolume %1$s on %2$s at %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the subvolume path (e.g. home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   // %3$s is replaced by the mount point (e.g. /home)
			   _("Mounting subvolume %1$s on %2$s at %3$s"));

	return sformat(text, path, get_message_name(), mount_point->get_path());
    }


    Text
    BtrfsSubvolume::Impl::do_unmount_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the subvolume path (e.g. home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   // %3$s is replaced by the mount point (e.g. /home)
			   _("Unmount subvolume %1$s on %2$s at %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the subvolume path (e.g. home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB)),
			   // %3$s is replaced by the mount point (e.g. /home)
			   _("Unmounting subvolume %1$s on %2$s at %3$s"));

	return sformat(text, path, get_message_name(), mount_point->get_path());
    }


    Text
    BtrfsSubvolume::Impl::do_add_to_etc_fstab_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by the subvolume path (e.g. home),
			   // %3$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Add mount point %1$s of subvolume %2$s on %3$s to /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by the subvolume path (e.g. home),
			   // %3$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Adding mount point %1$s of subvolume %2$s on %3$s to /etc/fstab"));

	return sformat(text, mount_point->get_path(), path, get_message_name());
    }


    Text
    BtrfsSubvolume::Impl::do_remove_from_etc_fstab_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by the subvolume path (e.g. home),
			   // %3$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Remove mount point %1$s of subvolume %2$s on %3$s from /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by the subvolume path (e.g. home),
			   // %3$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Removing mount point %1$s of subvolume %2$s on %3$s from /etc/fstab"));

	return sformat(text, mount_point->get_path(), path, get_message_name());
    }


    Text
    BtrfsSubvolume::Impl::do_set_nocow_text(Tense tense) const
    {
	Text text;

	if (is_nocow())
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by the subvolume path (e.g. var/log),
			  // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			  // and /dev/sdb2 (1.00 GiB))
			  _("Set option 'no copy on write' for subvolume %1$s on %2$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by the subvolume path (e.g. var/log),
			  // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			  // and /dev/sdb2 (1.00 GiB))
			  _("Setting option 'no copy on write' for subvolume %1$s on %2$s"));
	else
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by the subvolume path (e.g. var/log),
			  // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			  // and /dev/sdb2 (1.00 GiB))
			  _("Clear option 'no copy on write' for subvolume %1$s on %2$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by the subvolume path (e.g. var/log),
			  // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			  // and /dev/sdb2 (1.00 GiB))
			  _("Clearing option 'no copy on write' for subvolume %1$s on %2$s"));

	return sformat(text, get_displayname(), get_message_name());
    }


    void
    BtrfsSubvolume::Impl::do_set_nocow() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	EnsureMounted ensure_mounted(top_level, false);

	string cmd_line = CHATTR_BIN " " + string(nocow ? "+" : "-") + "C " +
	    quote(ensure_mounted.get_any_mount_point() + "/" + path);

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    BtrfsSubvolume::Impl::do_set_default_btrfs_subvolume_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the subvolume path (e.g. var/log),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Set default subvolume to subvolume %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the subvolume path (e.g. var/log),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Setting default subvolume to subvolume %1$s on %2$s"));

	return sformat(text, get_displayname(), get_message_name());
    }


    void
    BtrfsSubvolume::Impl::do_set_default_btrfs_subvolume() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	EnsureMounted ensure_mounted(top_level, false);

	string cmd_line = BTRFS_BIN " subvolume set-default " + to_string(id) + " " +
	    quote(ensure_mounted.get_any_mount_point());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    BtrfsSubvolume::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the subvolume path (e.g. var/log),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Delete subvolume %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the subvolume path (e.g. var/log),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Deleting subvolume %1$s on %2$s"));

	return sformat(text, path, get_message_name());
    }


    void
    BtrfsSubvolume::Impl::do_delete() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	EnsureMounted ensure_mounted(top_level, false);

	string cmd_line = BTRFS_BIN " subvolume delete " +
	    quote(ensure_mounted.get_any_mount_point() + "/" + path);

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    BtrfsSubvolume::Impl::do_pre_mount() const
    {
	get_btrfs()->get_impl().do_pre_mount();
    }


    namespace Action
    {

	Text
	SetNocow::text(const CommitData& commit_data) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_nocow_text(commit_data.tense);
	}


	void
	SetNocow::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    btrfs_subvolume->get_impl().do_set_nocow();
	}


	uf_t
	SetNocow::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_nocow_used_features();
	}


	Text
	SetDefaultBtrfsSubvolume::text(const CommitData& commit_data) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_default_btrfs_subvolume_text(commit_data.tense);
	}


	void
	SetDefaultBtrfsSubvolume::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    btrfs_subvolume->get_impl().do_set_default_btrfs_subvolume();
	}


	uf_t
	SetDefaultBtrfsSubvolume::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_default_btrfs_subvolume_used_features();
	}

    }

}
