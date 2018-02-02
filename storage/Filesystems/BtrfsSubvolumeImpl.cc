/*
 * Copyright (c) [2017-2018] SUSE LLC
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
#include "storage/EtcFstab.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<BtrfsSubvolume>::classname = "BtrfsSubvolume";


    const long BtrfsSubvolume::Impl::top_level_id;
    const long BtrfsSubvolume::Impl::unknown_id;


    BtrfsSubvolume::Impl::Impl(const xmlNode* node)
	: Mountable::Impl(node), id(unknown_id), path(), default_btrfs_subvolume(false), nocow(false)
    {
	if (!getChildValue(node, "id", id))
	    ST_THROW(Exception("no id"));

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

	setChildValue(node, "id", id);
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


    string
    BtrfsSubvolume::Impl::get_mount_name() const
    {
	return get_btrfs()->get_impl().get_mount_name();
    }


    string
    BtrfsSubvolume::Impl::get_mount_by_name(MountByType mount_by_type) const
    {
	return get_btrfs()->get_impl().get_mount_by_name(mount_by_type);
    }


    MountByType
    BtrfsSubvolume::Impl::get_default_mount_by() const
    {
	return get_btrfs()->get_impl().get_default_mount_by();
    }


    MountOpts
    BtrfsSubvolume::Impl::get_default_mount_options() const
    {
	return MountOpts(vector<string>({ "subvol=/" + path }));
    }


    vector<string>
    BtrfsSubvolume::Impl::get_mount_options() const
    {
	return vector<string>({ "subvol=/" + path });
    }


    vector<FstabEntry*>
    BtrfsSubvolume::Impl::find_etc_fstab_entries(EtcFstab& etc_fstab, const vector<string>& names) const
    {
	vector<FstabEntry*> ret;

	for (FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (fstab_entry->get_mount_opts().has_subvol(id, path))
		ret.push_back(fstab_entry);
	}

	return ret;
    }


    vector<const FstabEntry*>
    BtrfsSubvolume::Impl::find_etc_fstab_entries(const EtcFstab& etc_fstab, const vector<string>& names) const
    {
	vector<const FstabEntry*> ret;

	for (const FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (fstab_entry->get_mount_opts().has_subvol(id, path))
		ret.push_back(fstab_entry);
	}

	return ret;
    }


    vector<const FstabEntry*>
    BtrfsSubvolume::Impl::find_proc_mounts_entries(SystemInfo& system_info, const vector<string>& names) const
    {
	// see doc/btrfs.md for default id handling

	long default_id = get_btrfs()->get_default_btrfs_subvolume()->get_id();

	vector<const FstabEntry*> ret;

	for (const FstabEntry* mount_entry : system_info.getProcMounts().get_by_name(names[0], system_info))
	{
	    if (mount_entry->get_mount_opts().has_subvol(id, path) && id != default_id)
		ret.push_back(mount_entry);
	}

	return ret;
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
	SystemInfo& system_info = prober.get_system_info();

	const Btrfs* btrfs = get_btrfs();
	const BlkDevice* blk_device = btrfs->get_impl().get_blk_device();

	vector<string> aliases = EtcFstab::construct_device_aliases(blk_device, btrfs);

	vector<const FstabEntry*> fstab_entries = find_etc_fstab_entries(system_info.getEtcFstab(), aliases);
	vector<const FstabEntry*> mount_entries = find_proc_mounts_entries(system_info, aliases);

	// The code here works only with one mount point per
	// mountable. Anything else is not supported since rejected by the
	// product owner.

	vector<JointEntry> joint_entries = join_entries(fstab_entries, mount_entries);
	if (!joint_entries.empty())
	    joint_entries[0].add_to(get_non_impl());
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
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
			   _("Create subvolume %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
			   _("Creating subvolume %1$s on %2$s"));

	const BlkDevice* blk_device = get_btrfs()->get_impl().get_blk_device();

        return sformat(text, path.c_str(), blk_device->get_name().c_str());
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
	    createPath(full_dirname);

	string cmd_line = BTRFSBIN " subvolume create " + quote(full_path);

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create BtrfsSubvolume failed"));

	probe_id(ensure_mounted.get_any_mount_point());
    }


    Text
    BtrfsSubvolume::Impl::do_mount_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by subvolume path (e.g. home),
			   // %2$s is replaced by device name (e.g. /dev/sda1),
			   // %3$s is replaced by mount point (e.g. /home)
			   _("Mount subvolume %1$s on %2$s at %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by subvolume path (e.g. home),
			   // %2$s is replaced by device name (e.g. /dev/sda1),
			   // %3$s is replaced by mount point (e.g. /home)
			   _("Mounting subvolume %1$s on %2$s at %3$s"));

	return sformat(text, path.c_str(), get_mount_name().c_str(), mount_point->get_path().c_str());
    }


    Text
    BtrfsSubvolume::Impl::do_umount_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by subvolume path (e.g. home),
			  // %2$s is replaced by device name (e.g. /dev/sda1),
			  // %3$s is replaced by mount point (e.g. /home)
			  _("Unmount subvolume %1$s on %2$s at %3$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by subvolume path (e.g. home),
			  // %2$s is replaced by device name (e.g. /dev/sda1),
			  // %3$s is replaced by mount point (e.g. /home)
			  _("Unmounting subvolume %1$s on %2$s at %3$s"));

	return sformat(text, path.c_str(), get_mount_name().c_str(), mount_point->get_path().c_str());
    }


    Text
    BtrfsSubvolume::Impl::do_add_to_etc_fstab_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Add mount point %1$s of subvolume %2$s on %3$s to /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Adding mount point %1$s of subvolume %2$s on %3$s to /etc/fstab"));

	return sformat(text, mount_point->get_path().c_str(), path.c_str(), get_mount_name().c_str());
    }


    Text
    BtrfsSubvolume::Impl::do_remove_from_etc_fstab_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Remove mount point %1$s of subvolume %2$s on %3$s from /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Removing mount point %1$s of subvolume %2$s on %3$s from /etc/fstab"));

	return sformat(text, mount_point->get_path().c_str(), path.c_str(), get_mount_name().c_str());
    }


    Text
    BtrfsSubvolume::Impl::do_set_nocow_text(Tense tense) const
    {
	Text text;

	if (is_nocow())
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
			  _("Set option 'no copy on write' for subvolume %1$s on %2$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
			  _("Setting option 'no copy on write' for subvolume %1$s on %2$s"));
	else
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
			  _("Clear option 'no copy on write' for subvolume %1$s on %2$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by subvolume path (e.g. var/log),
			  // %2$s is replaced by block device name (e.g. /dev/sda1)
			  _("Clearing option 'no copy on write' for subvolume %1$s on %2$s"));

	const BlkDevice* blk_device = get_btrfs()->get_impl().get_blk_device();

        return sformat(text, get_displayname().c_str(), blk_device->get_name().c_str());
    }


    void
    BtrfsSubvolume::Impl::do_set_nocow() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	EnsureMounted ensure_mounted(top_level, false);

	string cmd_line = CHATTRBIN " " + string(nocow ? "+" : "-") + "C " +
	    quote(ensure_mounted.get_any_mount_point() + "/" + path);

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set nocow failed"));
    }


    Text
    BtrfsSubvolume::Impl::do_set_default_btrfs_subvolume_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
			   _("Set default subvolume to subvolume %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
			   _("Setting default subvolume to subvolume %1$s on %2$s"));

	const BlkDevice* blk_device = get_btrfs()->get_impl().get_blk_device();

        return sformat(text, get_displayname().c_str(), blk_device->get_name().c_str());
    }


    void
    BtrfsSubvolume::Impl::do_set_default_btrfs_subvolume() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	EnsureMounted ensure_mounted(top_level, false);

	string cmd_line = BTRFSBIN " subvolume set-default " + to_string(id) + " " +
	    quote(ensure_mounted.get_any_mount_point());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set default btrfs subvolume failed"));
    }


    Text
    BtrfsSubvolume::Impl::do_delete_text(Tense tense) const
    {
        Text text = tenser(tense,
                           // TRANSLATORS: displayed before action,
			   // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
                           _("Delete subvolume %1$s on %2$s"),
                           // TRANSLATORS: displayed during action,
			   // %1$s is replaced by subvolume path (e.g. var/log),
			   // %2$s is replaced by block device name (e.g. /dev/sda1)
                           _("Deleting subvolume %1$s on %2$s"));

	const BlkDevice* blk_device = get_btrfs()->get_impl().get_blk_device();

        return sformat(text, path.c_str(), blk_device->get_name().c_str());
    }


    void
    BtrfsSubvolume::Impl::do_delete() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	EnsureMounted ensure_mounted(top_level, false);

	string cmd_line = BTRFSBIN " subvolume delete " +
	    quote(ensure_mounted.get_any_mount_point() + "/" + path);

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete BtrfsSubvolume failed"));
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

    }

}
