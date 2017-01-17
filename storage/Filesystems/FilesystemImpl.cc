/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include <glob.h>
#include <iostream>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Filesystem>::classname = "Filesystem";


    // strings must match /etc/fstab
    const vector<string> EnumTraits<FsType>::names({
	"unknown", "reiserfs", "ext2", "ext3", "ext4", "btrfs", "vfat", "xfs", "jfs", "hfs",
	"ntfs", "swap", "hfsplus", "nfs", "nfs4", "tmpfs", "iso9660", "udf"
    });


    const vector<string> EnumTraits<MountByType>::names({
	"device", "uuid", "label", "id", "path"
    });


    Filesystem::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), label(), uuid(), mountpoints({}), mount_by(MountByType::DEVICE),
	  fstab_options({}), mkfs_options(), tune_options(), resize_info(), content_info()
    {
	string tmp;

	getChildValue(node, "label", label);
	getChildValue(node, "uuid", uuid);

	getChildValue(node, "mountpoint", mountpoints);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "fstab-options", tmp))
	    fstab_options = splitString(tmp, ",");

	getChildValue(node, "mkfs-options", mkfs_options);
	getChildValue(node, "tune-options", tune_options);

	const xmlNode* resize_info_node = getChildNode(node, "ResizeInfo");
	if (resize_info_node)
	    resize_info.set_value(ResizeInfo(resize_info_node));

	const xmlNode* content_info_node = getChildNode(node, "ContentInfo");
	if (content_info_node)
	    content_info.set_value(ContentInfo(content_info_node));
    }


    void
    Filesystem::Impl::set_label(const string& label)
    {
	Impl::label = label;
    }


    void
    Filesystem::Impl::set_uuid(const string& uuid)
    {
	Impl::uuid = uuid;
    }


    void
    Filesystem::Impl::set_mountpoints(const vector<string>& mountpoints)
    {
	Impl::mountpoints = mountpoints;
    }


    void
    Filesystem::Impl::add_mountpoint(const string& mountpoint)
    {
	Impl::mountpoints.push_back(mountpoint);
    }


    void
    Filesystem::Impl::set_mount_by(MountByType mount_by)
    {
	Impl::mount_by = mount_by;
    }


    void
    Filesystem::Impl::set_fstab_options(const list<string>& fstab_options)
    {
	Impl::fstab_options = fstab_options;
    }


    void
    Filesystem::Impl::set_mkfs_options(const string& mkfs_options)
    {
	Impl::mkfs_options = mkfs_options;
    }


    void
    Filesystem::Impl::set_tune_options(const string& tune_options)
    {
	Impl::tune_options = tune_options;
    }


    void
    Filesystem::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValueIf(node, "label", label, !label.empty());
	setChildValueIf(node, "uuid", uuid, !uuid.empty());

	setChildValue(node, "mountpoint", mountpoints);

	setChildValueIf(node, "mount-by", toString(mount_by), mount_by != MountByType::DEVICE);

	if (!fstab_options.empty())
	    setChildValue(node, "fstab-options", boost::join(fstab_options, ","));

	setChildValueIf(node, "mkfs-options", mkfs_options, !mkfs_options.empty());
	setChildValueIf(node, "tune-options", tune_options, !tune_options.empty());

	if (resize_info.has_value())
	{
	    xmlNode* resize_info_node = xmlNewChild(node, "ResizeInfo");
	    resize_info.get_value().save(resize_info_node);
	}

	if (content_info.has_value())
	{
	    xmlNode* content_info_node = xmlNewChild(node, "ContentInfo");
	    content_info.get_value().save(content_info_node);
	}
    }


    void
    Filesystem::Impl::probe_pass_3(Devicegraph* probed, SystemInfo& systeminfo, EtcFstab& fstab)
    {
	const BlkDevice* blk_device = get_blk_device();

	const Blkid& blkid = systeminfo.getBlkid();
	Blkid::Entry entry;
	if (blkid.find_by_name(blk_device->get_name(), entry, systeminfo))
	{
	    label = entry.fs_label;
	    uuid = entry.fs_uuid;
	}

	const ProcMounts& proc_mounts = systeminfo.getProcMounts();
	for (const string& mountpoint : proc_mounts.find_by_name(blk_device->get_name(), systeminfo))
	    mountpoints.push_back(mountpoint);

	fstab.setDevice(blk_device->get_name(), {}, uuid, label, blk_device->get_udev_ids(),
			blk_device->get_udev_paths());
	FstabEntry fstabentry;
	if (fstab.findDevice(blk_device->get_name(), fstabentry))
	{
	    mount_by = fstabentry.mount_by;
	    fstab_options = fstabentry.opts;
	}
    }


    ResizeInfo
    Filesystem::Impl::detect_resize_info() const
    {
	if (!resize_info.has_value())
	{
	    // TODO redirect to probed devicegraph

	    resize_info.set_value(detect_resize_info_pure());
	}

	return resize_info.get_value();
    }


    ResizeInfo
    Filesystem::Impl::detect_resize_info_pure() const
    {
	if (!get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	// TODO only in real probe mode allowed

	EnsureMounted ensure_mounted(this);

	StatVfs stat_vfs = detect_stat_vfs(ensure_mounted.get_any_mountpoint());

	ResizeInfo resize_info(true);
	resize_info.min_size = stat_vfs.size - stat_vfs.free;

	return resize_info;
    }


    void
    Filesystem::Impl::set_resize_info(const ResizeInfo& tmp)
    {
	resize_info.set_value(tmp);
    }


    ContentInfo
    Filesystem::Impl::detect_content_info() const
    {
	if (!content_info.has_value())
	{
	    // TODO redirect to probed devicegraph

	    content_info.set_value(detect_content_info_pure());
	}

	return content_info.get_value();
    }


    ContentInfo
    Filesystem::Impl::detect_content_info_pure() const
    {
	if (!get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	// TODO only in real probe mode allowed

	EnsureMounted ensure_mounted(this);

	ContentInfo content_info;
	content_info.is_windows = false;
	content_info.is_efi = false;
	content_info.num_homes = detect_num_homes(ensure_mounted.get_any_mountpoint());

	return content_info;
    }


    void
    Filesystem::Impl::set_content_info(const ContentInfo& tmp)
    {
	content_info.set_value(tmp);
    }


    bool
    Filesystem::Impl::detect_is_windows(const string& mountpoint)
    {
	// The file '$Boot' is special. It is a reserved name of NTFS and from
	// linux not visible with 'ls /mnt' but with 'ls /mnt/$Boot'.

	const char* files[] = { "boot.ini", "msdos.sys", "io.sys", "config.sys", "MSDOS.SYS",
				"IO.SYS", "bootmgr", "$Boot" };

	for (unsigned int i = 0; i < lengthof(files); ++i)
	{
	    string file = mountpoint + "/" + files[i];
	    if (access(file.c_str(), R_OK) == 0)
	    {
		y2mil("found windows file " << quote(file));
		return true;
	    }
	}

	return false;
    }


    bool
    Filesystem::Impl::detect_is_efi(const string& mountpoint)
    {
	return checkDir(mountpoint + "/efi");
    }


    unsigned
    Filesystem::Impl::detect_num_homes(const string& mountpoint)
    {
	const char* files[] = { ".profile", ".bashrc", ".ssh", ".kde", ".kde4", ".gnome",
				".gnome2" };

	unsigned num_homes = 0;

	const vector<string> dirs = glob(mountpoint + "/*", GLOB_NOSORT | GLOB_ONLYDIR);
	for (const string& dir : dirs)
	{
	    if (!boost::ends_with(dir, "/root") && checkDir(dir))
	    {
		for (unsigned int i = 0; i < lengthof(files); ++i)
		{
		    string file = dir + "/" + files[i];
		    if (access(file.c_str(), R_OK) == 0)
		    {
			y2mil("found home file " << quote(file));
			++num_homes;
			break;
		    }
		}
	    }

	    if (num_homes >= 2)
		return num_homes;
	}

	return num_homes;
    }


    void
    Filesystem::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	Action::Base* first = nullptr;
	Action::Base* last = nullptr;

	Action::Create* format = new Action::Create(get_sid());
	Actiongraph::Impl::vertex_descriptor v1 = actiongraph.add_vertex(format);
	first = last = format;

	if (!get_label().empty())
	{
	    Action::SetLabel* set_label = new Action::SetLabel(get_sid());
	    Actiongraph::Impl::vertex_descriptor tmp = actiongraph.add_vertex(set_label);
	    actiongraph.add_edge(v1, tmp);
	    v1 = tmp;

	    last = set_label;
	}

	if (!get_uuid().empty())
	{
	    Action::SetUuid* set_uuid = new Action::SetUuid(get_sid());
	    Actiongraph::Impl::vertex_descriptor tmp2 = actiongraph.add_vertex(set_uuid);
	    actiongraph.add_edge(v1, tmp2);
	    v1 = tmp2;

	    last = set_uuid;
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

		Action::AddEtcFstab* add_etc_fstab = new Action::AddEtcFstab(get_sid(), mountpoint);
		Actiongraph::Impl::vertex_descriptor t2 = actiongraph.add_vertex(add_etc_fstab);

		actiongraph.add_edge(v1, t1);
		actiongraph.add_edge(t1, t2);
		actiongraph.add_edge(t2, v2);
	    }
	}

	first->first = true;
	last->last = true;
    }


    void
    Filesystem::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());
	vector<Action::Base*> actions;

	if (get_type() != lhs.get_type())
	{
	    ST_THROW(Exception("cannot change filesystem type"));
	}

	if (get_label() != lhs.get_label())
	{
	    actions.push_back(new Action::SetLabel(get_sid()));
	}

	if (get_uuid() != lhs.get_uuid())
	{
	    actions.push_back(new Action::SetUuid(get_sid()));
	}

	// TODO depends on mount-by, whether there actually is an entry in fstab,
	// btrfs multiple devices, ...
	if (get_blk_device()->get_name() != lhs.get_blk_device()->get_name())
	{
	    for (const string& mountpoint : get_mountpoints())
		actions.push_back(new Action::RenameEtcFstab(get_sid(), mountpoint));
	}

	actiongraph.add_chain(actions);
    }


    void
    Filesystem::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	Action::Base* first = nullptr;
	Action::Base* last = nullptr;

	Action::Base* sync1 = new Action::Delete(get_sid(), true);
	Actiongraph::Impl::vertex_descriptor v1 = actiongraph.add_vertex(sync1);
	first = last = sync1;

	if (!get_mountpoints().empty())
	{
	    Action::Base* sync2 = new Action::Delete(get_sid(), true);
	    Actiongraph::Impl::vertex_descriptor v2 = actiongraph.add_vertex(sync2);
	    first = sync2;

	    for (const string& mountpoint : get_mountpoints())
	    {
		Action::RemoveEtcFstab* remove_etc_fstab = new Action::RemoveEtcFstab(get_sid(), mountpoint);
		Actiongraph::Impl::vertex_descriptor t1 = actiongraph.add_vertex(remove_etc_fstab);

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


    vector<const BlkDevice*>
    Filesystem::Impl::get_blk_devices() const
    {
	const Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<BlkDevice>(devicegraph->get_impl().parents(vertex));
    }


    const BlkDevice*
    Filesystem::Impl::get_blk_device() const
    {
	vector<const BlkDevice*> blk_devices = get_blk_devices();
	if (blk_devices.size() != 1)
	    throw;

	return blk_devices.front();
    }


    bool
    Filesystem::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return label == rhs.label && uuid == rhs.uuid && mountpoints == rhs.mountpoints &&
	    mount_by == rhs.mount_by && fstab_options == rhs.fstab_options &&
	    mkfs_options == rhs.mkfs_options && tune_options == rhs.tune_options;
    }


    void
    Filesystem::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "label", label, rhs.label);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "mountpoints", mountpoints, rhs.mountpoints);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

	storage::log_diff(log, "fstab-options", fstab_options, rhs.fstab_options);

	storage::log_diff(log, "mkfs-options", mkfs_options, rhs.mkfs_options);
	storage::log_diff(log, "tune-options", tune_options, rhs.tune_options);
    }


    void
    Filesystem::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	if (!label.empty())
	    out << " label:" << label;

	if (!uuid.empty())
	    out << " uuid:" << uuid;

	if (!mountpoints.empty())
	    out << " mountpoints:" << mountpoints;

	if (!fstab_options.empty())
	    out << " fstab-options:" << fstab_options;

	if (!mkfs_options.empty())
	    out << " mkfs-options:" << mkfs_options;

	if (!tune_options.empty())
	    out << " tune-options:" << tune_options;
    }


    string
    Filesystem::Impl::get_mount_by_string() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string ret = blk_device->get_name();

	switch (mount_by)
	{
	    case MountByType::UUID:
		if (!uuid.empty())
		    ret = "UUID=" + uuid;
		else
		    y2err("no uuid defined");
		break;

	    case MountByType::LABEL:
		if (!label.empty())
		    ret = "LABEL=" + label;
		else
		    y2err("no label defined");
		break;

	    case MountByType::ID:
		if (!blk_device->get_udev_ids().empty())
		    ret = DEVDIR "/disk/by-id/" + blk_device->get_udev_ids().front();
		else
		    y2err("no udev-id defined");
		break;

	    case MountByType::PATH:
		if (!blk_device->get_udev_paths().empty())
		    ret = DEVDIR "/disk/by-path/" + blk_device->get_udev_paths().front();
		else
		    y2err("no udev-path defined");
		break;

	    case MountByType::DEVICE:
		break;
	}

	return ret;
    }


    Text
    Filesystem::Impl::do_create_text(Tense tense) const
    {
	// TODO handle multiple BlkDevices

	const BlkDevice* blk_device = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by filesystem name (e.g. ext4),
			   // %2$s is replaced by device name (e.g. /dev/sda1),
			   // %3$s is replaced by size (e.g. 2GiB)
			   _("Create %1$s on %2$s (%3$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by filesystem name (e.g. ext4),
			   // %2$s is replaced by device name (e.g. /dev/sda1),
			   // %3$s is replaced by size (e.g. 2GiB)
			   _("Creating %1$s on %2$s (%3$s)"));

	return sformat(text, get_displayname().c_str(), blk_device->get_name().c_str(),
		       blk_device->get_size_string().c_str());
    }


    Text
    Filesystem::Impl::do_set_label_text(Tense tense) const
    {
	const BlkDevice* blk_device = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by label (e.g. ROOT)
			   _("Set label of %1$s to %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by label (e.g. ROOT)
			   _("Setting label of %1$s to %2$s"));

	return sformat(text, blk_device->get_name().c_str(), label.c_str());
    }


    void
    Filesystem::Impl::do_set_label() const
    {
	ST_THROW(LogicException("stub do_set_label called"));
    }


    Text
    Filesystem::Impl::do_set_uuid_text(Tense tense) const
    {
	const BlkDevice* blk_device = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by UUID (e.g. 3cfa63b5-4d29-43e6-8658-57b74f68fd7f)
			   _("Set UUID of %1$s to %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by UUID (e.g. 3cfa63b5-4d29-43e6-8658-57b74f68fd7f)
			   _("Setting UUID of %1$s to %2$s"));

	return sformat(text, blk_device->get_name().c_str(), uuid.c_str());
    }


    void
    Filesystem::Impl::do_set_uuid() const
    {
	ST_THROW(LogicException("stub do_set_uuid called"));
    }


    Text
    Filesystem::Impl::do_mount_text(const string& mountpoint, Tense tense) const
    {
	const BlkDevice* blk_device = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Mount %1$s at %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Mounting %1$s at %2$s"));

	return sformat(text, blk_device->get_name().c_str(), mountpoint.c_str());
    }


    void
    Filesystem::Impl::do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const BlkDevice* blk_device = get_blk_device();

	const Storage& storage = actiongraph.get_storage();

	string real_mountpoint = storage.get_impl().prepend_rootprefix(mountpoint);
	if (access(real_mountpoint.c_str(), R_OK ) != 0)
	{
	    createPath(real_mountpoint);
	}

	string cmd_line = MOUNTBIN " -t " + toString(get_type()) + " " +
	    quote(blk_device->get_name()) + " " + quote(real_mountpoint);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("mount failed"));
    }


    Text
    Filesystem::Impl::do_umount_text(const string& mountpoint, Tense tense) const
    {
	const BlkDevice* blk_device = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Unmount %1$s at %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Unmounting %1$s at %2$s"));

	return sformat(text, blk_device->get_name().c_str(), mountpoint.c_str());
    }


    void
    Filesystem::Impl::do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	string real_mountpoint = storage.get_impl().prepend_rootprefix(mountpoint);

	string cmd_line = UMOUNTBIN " " + quote(real_mountpoint);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("umount failed"));
    }


    Text
    Filesystem::Impl::do_add_etc_fstab_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Add mountpoint %1$s of %2$s to /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Adding mountpoint %1$s of %2$s to /etc/fstab"));

	return sformat(text, mountpoint.c_str(), get_blk_device()->get_name().c_str());
    }


    void
    Filesystem::Impl::do_add_etc_fstab(const Actiongraph::Impl& actiongraph,
				       const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab(storage.get_impl().prepend_rootprefix("/etc"));	// TODO pass as parameter

	const BlkDevice* blk_device = get_blk_device();

	// TODO

	FstabChange entry;
	entry.device = blk_device->get_name();
	entry.dentry = get_mount_by_string();
	entry.mount = mountpoint;
	entry.fs = toString(get_type());
	entry.opts = fstab_options;

	fstab.addEntry(entry);
	fstab.flush();
    }


    Text
    Filesystem::Impl::do_rename_etc_fstab_text(const Device* lhs, const string& mountpoint,
					       Tense tense) const
    {
	const BlkDevice* blk_device_lhs = to_filesystem(lhs)->get_impl().get_blk_device();
	const BlkDevice* blk_device_rhs = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda6),
			   // %3$s is replaced by device name (e.g. /dev/sda5)
			   _("Rename mountpoint %1$s from %2$s to %3$s in /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda6),
			   // %3$s is replaced by device name (e.g. /dev/sda5)
			   _("Renaming mountpoint %1$s from %2$s to %3$s in /etc/fstab"));

	return sformat(text, mountpoint.c_str(), blk_device_lhs->get_name().c_str(),
		       blk_device_rhs->get_name().c_str());
    }


    void
    Filesystem::Impl::do_rename_etc_fstab(const Actiongraph::Impl& actiongraph, const Device* lhs,
					  const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab(storage.get_impl().prepend_rootprefix("/etc"));	// TODO pass as parameter

	const BlkDevice* blk_device_lhs = to_filesystem(lhs)->get_impl().get_blk_device();

	// TODO

	FstabChange entry;
	entry.device = blk_device_lhs->get_name();
	entry.dentry = get_mount_by_string();
	entry.mount = mountpoint;
	entry.fs = toString(get_type());
	entry.opts = fstab_options;

	fstab.addEntry(entry);
	fstab.flush();
    }


    Text
    Filesystem::Impl::do_remove_etc_fstab_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Remove mountpoint %1$s of %2$s from /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mountpoint (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Removing mountpoint %1$s of %2$s from /etc/fstab"));

	return sformat(text, mountpoint.c_str(), get_blk_device()->get_name().c_str());
    }


    void
    Filesystem::Impl::do_remove_etc_fstab(const Actiongraph::Impl& actiongraph,
					  const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab(storage.get_impl().prepend_rootprefix("/etc"));	// TODO pass as parameter

	const BlkDevice* blk_device = get_blk_device();

	// TODO error handling

	FstabKey entry(blk_device->get_name(), mountpoint);
	fstab.removeEntry(entry);
	fstab.flush();
    }


    Text
    Filesystem::Impl::do_resize_text(ResizeMode resize_mode, const Device* lhs, Tense tense) const
    {
	const BlkDevice* blk_device_lhs = to_filesystem(lhs)->get_impl().get_blk_device();
	const BlkDevice* blk_device_rhs = get_blk_device();

	Text text;

	switch (resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by filesystem (e.g. ext4),
			      // %2$s is replaced by device name (e.g. /dev/sda1),
			      // %3$s is replaced by old size (e.g. 2GiB),
			      // %4$s is replaced by new size (e.g. 1GiB)
			      _("Shrink %1$s on %2$s from %3$s to %4$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by filesystem (e.g. ext4),
			      // %2$s is replaced by device name (e.g. /dev/sda1),
			      // %3$s is replaced by old size (e.g. 2GiB),
			      // %4$s is replaced by new size (e.g. 1GiB)
			      _("Shrinking %1$s on %2$s from %3$s to %4$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by filesystem (e.g. ext4),
			      // %2$s is replaced by device name (e.g. /dev/sda1),
			      // %3$s is replaced by old size (e.g. 1GiB),
			      // %4$s is replaced by new size (e.g. 2GiB)
			      _("Grow %1$s on %2$s from %3$s to %4$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by filesystem (e.g. ext4),
			      // %2$s is replaced by device name (e.g. /dev/sda1),
			      // %3$s is replaced by old size (e.g. 1GiB),
			      // %4$s is replaced by new size (e.g. 2GiB)
			      _("Growing %1$s on %2$s from %3$s to %4$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, get_displayname().c_str(), blk_device_rhs->get_name().c_str(),
		       blk_device_lhs->get_size_string().c_str(),
		       blk_device_rhs->get_size_string().c_str());
    }


    namespace Action
    {

	Text
	SetLabel::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    return filesystem->get_impl().do_set_label_text(tense);
	}


	void
	SetLabel::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    filesystem->get_impl().do_set_label();
	}


	Text
	SetUuid::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    return filesystem->get_impl().do_set_uuid_text(tense);
	}


	void
	SetUuid::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    filesystem->get_impl().do_set_uuid();
	}


	Text
	Mount::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    return filesystem->get_impl().do_mount_text(mountpoint, tense);
	}


	void
	Mount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    filesystem->get_impl().do_mount(actiongraph, mountpoint);
	}


	Text
	Umount::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_lhs(actiongraph));
	    return filesystem->get_impl().do_umount_text(mountpoint, tense);
	}


	void
	Umount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_lhs(actiongraph));
	    filesystem->get_impl().do_umount(actiongraph, mountpoint);
	}


	Text
	AddEtcFstab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    return filesystem->get_impl().do_add_etc_fstab_text(mountpoint, tense);
	}


	void
	AddEtcFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_rhs(actiongraph));
	    filesystem->get_impl().do_add_etc_fstab(actiongraph, mountpoint);
	}


	void
	AddEtcFstab::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
				      Actiongraph::Impl& actiongraph) const
	{
	    if (mountpoint == "swap")
		if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		    actiongraph.add_edge(*actiongraph.mount_root_filesystem, vertex);
	}


	Text
	RenameEtcFstab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem_lhs = to_filesystem(get_device_lhs(actiongraph));
	    const Filesystem* filesystem_rhs = to_filesystem(get_device_rhs(actiongraph));
	    return filesystem_rhs->get_impl().do_rename_etc_fstab_text(filesystem_lhs, mountpoint, tense);
	}


	void
	RenameEtcFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem_lhs = to_filesystem(get_device_lhs(actiongraph));
	    const Filesystem* filesystem_rhs = to_filesystem(get_device_rhs(actiongraph));
	    filesystem_rhs->get_impl().do_rename_etc_fstab(actiongraph, filesystem_lhs, mountpoint);
	}


	Text
	RemoveEtcFstab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_lhs(actiongraph));
	    return filesystem->get_impl().do_remove_etc_fstab_text(mountpoint, tense);
	}


	void
	RemoveEtcFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device_lhs(actiongraph));
	    filesystem->get_impl().do_remove_etc_fstab(actiongraph, mountpoint);
	}

    }


    EnsureMounted::EnsureMounted(const Filesystem::Impl* filesystem)
	: filesystem(filesystem), tmp_mount()
    {
	if (!filesystem->get_mountpoints().empty())
	    return;

	const Storage* storage = filesystem->get_storage();
	const BlkDevice* blk_device = filesystem->get_blk_device();

	blk_device->get_impl().wait_for_device();

	tmp_mount.reset(new TmpMount(storage->get_impl().get_tmp_dir().get_fullname(),
				     "tmp-mount-XXXXXX", blk_device->get_name()));
    }


    string
    EnsureMounted::get_any_mountpoint() const
    {
	if (tmp_mount)
	    return tmp_mount->get_fullname();
	else
	    return filesystem->get_mountpoints().front();
    }

}
