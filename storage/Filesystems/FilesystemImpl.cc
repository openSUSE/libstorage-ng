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
#include "storage/Filesystems/BlkFilesystemImpl.h"
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
	"ntfs", "swap", "hfsplus", "nfs", "nfs4", "tmpfs", "iso9660", "udf", "nilfs2"
    });


    const vector<string> EnumTraits<MountByType>::names({
	"device", "uuid", "label", "id", "path"
    });


    Filesystem::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), mountpoints({}), mount_by(MountByType::DEVICE), fstab_options({})
    {
	string tmp;

	getChildValue(node, "mountpoint", mountpoints);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "fstab-options", tmp))
	    fstab_options = splitString(tmp, ",");

	const xmlNode* space_info_node = getChildNode(node, "SpaceInfo");
	if (space_info_node)
	    space_info.set_value(SpaceInfo(space_info_node));
    }


    void
    Filesystem::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "mountpoint", mountpoints);

	setChildValueIf(node, "mount-by", toString(mount_by), mount_by != MountByType::DEVICE);

	if (!fstab_options.empty())
	    setChildValue(node, "fstab-options", boost::join(fstab_options, ","));

	if (space_info.has_value())
	{
	    xmlNode* space_info_node = xmlNewChild(node, "SpaceInfo");
	    space_info.get_value().save(space_info_node);
	}
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


    bool
    Filesystem::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return mountpoints == rhs.mountpoints && mount_by == rhs.mount_by &&
	    fstab_options == rhs.fstab_options;
    }


    void
    Filesystem::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "mountpoints", mountpoints, rhs.mountpoints);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

	storage::log_diff(log, "fstab-options", fstab_options, rhs.fstab_options);
    }


    SpaceInfo
    Filesystem::Impl::detect_space_info() const
    {
	if (!space_info.has_value())
	{
	    EnsureMounted ensure_mounted(get_filesystem());

	    SystemInfo systeminfo;
	    const CmdDf& cmd_df = systeminfo.getCmdDf(ensure_mounted.get_any_mountpoint());
	    space_info.set_value(cmd_df.get_space_info());
	}

	return space_info.get_value();
    }


    void
    Filesystem::Impl::set_space_info(const SpaceInfo& tmp)
    {
	space_info.set_value(tmp);
    }


    void
    Filesystem::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	if (!mountpoints.empty())
	    out << " mountpoints:" << mountpoints;

	if (!fstab_options.empty())
	    out << " fstab-options:" << fstab_options;
    }


    Text
    Filesystem::Impl::do_mount_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Mount %1$s at %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Mounting %1$s at %2$s"));

	return sformat(text, get_mount_string().c_str(), mountpoint.c_str());
    }


    void
    Filesystem::Impl::do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	string real_mountpoint = storage.get_impl().prepend_rootprefix(mountpoint);
	if (access(real_mountpoint.c_str(), R_OK ) != 0)
	{
	    createPath(real_mountpoint);
	}

	string cmd_line = MOUNTBIN " -t " + toString(get_type()) + " " +
	    quote(get_mount_string()) + " " + quote(real_mountpoint);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("mount failed"));
    }


    Text
    Filesystem::Impl::do_umount_text(const string& mountpoint, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Unmount %1$s at %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Unmounting %1$s at %2$s"));

	return sformat(text, get_mount_string().c_str(), mountpoint.c_str());
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
    Filesystem::Impl::do_add_to_etc_fstab_text(const string& mountpoint, Tense tense) const
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

	return sformat(text, mountpoint.c_str(), get_mount_string().c_str());
    }


    void
    Filesystem::Impl::do_add_to_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab(storage.get_impl().prepend_rootprefix("/etc"));	// TODO pass as parameter

	// TODO

	FstabChange entry;
	entry.device = get_mount_string();
	entry.dentry = get_mount_by_string();
	entry.mount = mountpoint;
	entry.fs = toString(get_type());
	entry.opts = get_fstab_options();

	fstab.addEntry(entry);
	fstab.flush();
    }


    Text
    Filesystem::Impl::do_remove_from_etc_fstab_text(const string& mountpoint, Tense tense) const
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

	return sformat(text, mountpoint.c_str(), get_mount_string().c_str());
    }


    void
    Filesystem::Impl::do_remove_from_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab(storage.get_impl().prepend_rootprefix("/etc"));	// TODO pass as parameter

	// TODO error handling

	FstabKey entry(get_mount_string(), mountpoint);
	fstab.removeEntry(entry);
	fstab.flush();
    }


    namespace Action
    {

	Text
	Mount::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, RHS));
	    return filesystem->get_impl().do_mount_text(mountpoint, tense);
	}


	void
	Mount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, RHS));
	    filesystem->get_impl().do_mount(actiongraph, mountpoint);
	}


	Text
	Umount::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, LHS));
	    return filesystem->get_impl().do_umount_text(mountpoint, tense);
	}


	void
	Umount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, LHS));
	    filesystem->get_impl().do_umount(actiongraph, mountpoint);
	}


	Text
	AddToEtcFstab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, RHS));
	    return filesystem->get_impl().do_add_to_etc_fstab_text(mountpoint, tense);
	}


	void
	AddToEtcFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, RHS));
	    filesystem->get_impl().do_add_to_etc_fstab(actiongraph, mountpoint);
	}


	void
	AddToEtcFstab::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
	    if (mountpoint == "swap")
		if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		    actiongraph.add_edge(*actiongraph.mount_root_filesystem, vertex);
	}


	Text
	RemoveFromEtcFstab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, LHS));
	    return filesystem->get_impl().do_remove_from_etc_fstab_text(mountpoint, tense);
	}


	void
	RemoveFromEtcFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(get_device(actiongraph, LHS));
	    filesystem->get_impl().do_remove_from_etc_fstab(actiongraph, mountpoint);
	}

    }


    EnsureMounted::EnsureMounted(const Filesystem* filesystem)
	: filesystem(filesystem), tmp_mount()
    {
	if (!filesystem->get_mountpoints().empty())
	    return;

	const Storage* storage = filesystem->get_impl().get_storage();

	if (is_blk_filesystem(filesystem))
	{
	    const BlkFilesystem* blk_filesystem = to_blk_filesystem(filesystem);
	    for (const BlkDevice* blk_device : blk_filesystem->get_blk_devices())
		blk_device->get_impl().wait_for_device();
	}

	tmp_mount.reset(new TmpMount(storage->get_impl().get_tmp_dir().get_fullname(),
				     "tmp-mount-XXXXXX", filesystem->get_impl().get_mount_string()));
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
