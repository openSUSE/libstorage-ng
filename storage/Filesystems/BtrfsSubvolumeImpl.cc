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


#include <iostream>

#include "storage/Utils/XmlFile.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
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
	BtrfsSubvolume* ret = to_btrfs_subvolume(get_device());

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
	const BtrfsSubvolume* ret = to_btrfs_subvolume(get_device());

	while (true)
	{
	    if (ret->get_id() == top_level_id)
		return ret;

	    ret = ret->get_impl().get_single_parent_of_type<const BtrfsSubvolume>();
	}
    }


    string
    BtrfsSubvolume::Impl::get_mount_name() const
    {
	return get_btrfs()->get_impl().get_mount_name();
    }


    string
    BtrfsSubvolume::Impl::get_mount_by_name() const
    {
	return get_btrfs()->get_impl().get_mount_by_name();
    }


    vector<string>
    BtrfsSubvolume::Impl::get_mount_options() const
    {
	return vector<string>({ "subvol=/" + path });
    }


    FstabEntry*
    BtrfsSubvolume::Impl::find_etc_fstab_entry(EtcFstab& etc_fstab, const vector<string>& names) const
    {
	for (FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (fstab_entry->get_mount_opts().has_subvol(id, path))
		return fstab_entry;
	}

	return nullptr;
    }


    const FstabEntry*
    BtrfsSubvolume::Impl::find_etc_fstab_entry(const EtcFstab& etc_fstab, const vector<string>& names) const
    {
	for (const FstabEntry* fstab_entry : etc_fstab.find_all_devices(names))
	{
	    if (fstab_entry->get_mount_opts().has_subvol(id, path))
		return fstab_entry;
	}

	return nullptr;
    }


    BtrfsSubvolume*
    BtrfsSubvolume::Impl::create_btrfs_subvolume(const string& path)
    {
	// TODO checks, e.g. path?

	Devicegraph* devicegraph = get_devicegraph();

	BtrfsSubvolume* btrfs_subvolume = create(devicegraph, path);
	btrfs_subvolume->set_mount_opts(vector<string>({ "subvol=/" + path }));

	Subdevice::create(devicegraph, get_device(), btrfs_subvolume);

	return btrfs_subvolume;
    }


    void
    BtrfsSubvolume::Impl::probe_pass_3(Devicegraph* probed, SystemInfo& systeminfo, const EtcFstab& etc_fstab,
				       const string& mountpoint)
    {
	const Btrfs* btrfs = get_btrfs();
	const BlkDevice* blk_device = btrfs->get_impl().get_blk_device();

	vector<string> aliases = EtcFstab::construct_device_aliases(blk_device, btrfs);

	const FstabEntry* fstab_entry = find_etc_fstab_entry(etc_fstab, aliases);
	if (fstab_entry)
        {
	    add_mountpoint(fstab_entry->get_mount_point());
	    set_fstab_device_name(fstab_entry->get_device());
	    set_mount_by(EtcFstab::get_mount_by(fstab_entry->get_device()));
	    set_mount_opts(fstab_entry->get_mount_opts());
	}

	const CmdLsattr& cmdlsattr = systeminfo.getCmdLsattr(blk_device->get_name(), mountpoint, path);
	nocow = cmdlsattr.is_nocow();
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
        Action::Base* first = nullptr;
        Action::Base* last = nullptr;

        Action::Create* format = new Action::Create(get_sid(), is_top_level());
        Actiongraph::Impl::vertex_descriptor v1 = actiongraph.add_vertex(format);
        first = last = format;

	// TODO complicated code. maybe it would be better to have mountpoints
	// as separate objects in the devicegraph to avoid the "non-linear"
	// dependency flows here.

	if (nocow)
	{
	    Action::Base* action = new Action::SetNocow(get_sid());
	    Actiongraph::Impl::vertex_descriptor t = actiongraph.add_vertex(action);

	    actiongraph.add_edge(v1, t);

	    v1 = t;
	    last = action;
	}

	if (default_btrfs_subvolume && !is_top_level())
	{
	    Action::Base* action = new Action::SetDefaultBtrfsSubvolume(get_sid());
	    Actiongraph::Impl::vertex_descriptor t = actiongraph.add_vertex(action);

	    actiongraph.add_edge(v1, t);

	    v1 = t;
	    last = action;
	}

        if (!get_mountpoints().empty())
        {
            Action::Base* sync = new Action::Create(get_sid(), true);
            Actiongraph::Impl::vertex_descriptor v2 = actiongraph.add_vertex(sync);
            last = sync;

            for (const string& mountpoint : get_mountpoints())
            {
                Action::Mount* mount = new Action::Mount(get_sid(), mountpoint);
                Actiongraph::Impl::vertex_descriptor t1 = actiongraph.add_vertex(mount);

                Action::AddToEtcFstab* add_to_etc_fstab = new Action::AddToEtcFstab(get_sid(), mountpoint);
                Actiongraph::Impl::vertex_descriptor t2 = actiongraph.add_vertex(add_to_etc_fstab);

                actiongraph.add_edge(v1, t1);
                actiongraph.add_edge(t1, t2);
                actiongraph.add_edge(t2, v2);
            }
        }

        first->first = true;
        last->last = true;
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
        Action::Base* first = nullptr;
        Action::Base* last = nullptr;

        Action::Base* sync1 = new Action::Delete(get_sid(), is_top_level());
        Actiongraph::Impl::vertex_descriptor v1 = actiongraph.add_vertex(sync1);
        first = last = sync1;

        if (!get_mountpoints().empty())
        {
            Action::Base* sync2 = new Action::Delete(get_sid(), true);
            Actiongraph::Impl::vertex_descriptor v2 = actiongraph.add_vertex(sync2);
            first = sync2;

            for (const string& mountpoint : get_mountpoints())
            {
                Action::RemoveFromEtcFstab* remove_from_etc_fstab = new Action::RemoveFromEtcFstab(get_sid(), mountpoint);
                Actiongraph::Impl::vertex_descriptor t1 = actiongraph.add_vertex(remove_from_etc_fstab);

                Action::Umount* umount = new Action::Umount(get_sid(), mountpoint);
                Actiongraph::Impl::vertex_descriptor t2 = actiongraph.add_vertex(umount);

                actiongraph.add_edge(v2, t1);
                actiongraph.add_edge(t1, t2);
                actiongraph.add_edge(t2, v1);
            }
        }

        first->first = true;
        last->last = true;
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
    BtrfsSubvolume::Impl::do_create() const
    {
	const BtrfsSubvolume* top_level = get_top_level_btrfs_subvolume();

	// TODO It is not always required to mount to top-level subvolume,
	// e.g. when creating <fs-tree>/a/b it is enough when subvol=a is
	// mounted somewhere.

	// TODO Just do one mount if several subvolumes get created.

	EnsureMounted ensure_mounted(top_level, false);

	string cmd_line = BTRFSBIN " subvolume create " +
	    quote(ensure_mounted.get_any_mountpoint() + "/" + path);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create BtrfsSubvolume failed"));
    }


    Text
    BtrfsSubvolume::Impl::do_mount_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by subvolume path (e.g. home),
			   // %2$s is replaced by device name (e.g. /dev/sda1),
			   // %3$s is replaced by mountpoint (e.g. /home)
			   _("Mount subvolume %1$s on %2$s at %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by subvolume path (e.g. home),
			   // %2$s is replaced by device name (e.g. /dev/sda1),
			   // %3$s is replaced by mountpoint (e.g. /home)
			   _("Mounting subvolume %1$s on %2$s at %3$s"));

	return sformat(text, path.c_str(), get_mount_name().c_str(), mountpoint.c_str());
    }


    Text
    BtrfsSubvolume::Impl::do_umount_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by subvolume path (e.g. home),
			  // %2$s is replaced by device name (e.g. /dev/sda1),
			  // %3$s is replaced by mountpoint (e.g. /home)
			  _("Unmount subvolume %1$s on %2$s at %3$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by subvolume path (e.g. home),
			  // %2$s is replaced by device name (e.g. /dev/sda1),
			  // %3$s is replaced by mountpoint (e.g. /home)
			  _("Unmounting subvolume %1$s on %2$s at %3$s"));

	return sformat(text, path.c_str(), get_mount_name().c_str(), mountpoint.c_str());
    }


    Text
    BtrfsSubvolume::Impl::do_add_to_etc_fstab_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Add mountpoint %1$s of subvolume %2$s on %3$s to /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Adding mountpoint %1$s of subvolume %2$s on %3$s to /etc/fstab"));

	return sformat(text, mountpoint.c_str(), path.c_str(), get_mount_name().c_str());
    }


    Text
    BtrfsSubvolume::Impl::do_remove_from_etc_fstab_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Remove mountpoint %1$s of subvolume %2$s on %3$s from /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by subvolume path (e.g. home),
			   // %3$s is replaced by device name (e.g. /dev/sda1)
			   _("Removing mountpoint %1$s of subvolume %2$s on %3$s from /etc/fstab"));

	return sformat(text, mountpoint.c_str(), path.c_str(), get_mount_name().c_str());
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
	    quote(ensure_mounted.get_any_mountpoint() + "/" + path);
	cout << cmd_line << endl;

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
	    quote(ensure_mounted.get_any_mountpoint());
	cout << cmd_line << endl;

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
	    quote(ensure_mounted.get_any_mountpoint() + "/" + path);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete BtrfsSubvolume failed"));
    }


    namespace Action
    {

	Text
	SetNocow::text(const CommitData& commit_data, Tense tense) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_nocow_text(tense);
	}


	void
	SetNocow::commit(CommitData& commit_data) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    btrfs_subvolume->get_impl().do_set_nocow();
	}


	Text
	SetDefaultBtrfsSubvolume::text(const CommitData& commit_data, Tense tense) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_default_btrfs_subvolume_text(tense);
	}


	void
	SetDefaultBtrfsSubvolume::commit(CommitData& commit_data) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    btrfs_subvolume->get_impl().do_set_default_btrfs_subvolume();
	}

    }

}
