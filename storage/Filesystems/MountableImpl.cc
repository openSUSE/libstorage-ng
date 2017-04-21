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


#include <iostream>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Filesystems/MountableImpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"
#include "storage/Redirect.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Mountable>::classname = "Mountable";


    // strings must match /etc/fstab
    const vector<string> EnumTraits<FsType>::names({
	"unknown", "reiserfs", "ext2", "ext3", "ext4", "btrfs", "vfat", "xfs", "jfs", "hfs",
	"ntfs", "swap", "hfsplus", "nfs", "nfs4", "tmpfs", "iso9660", "udf", "nilfs2"
    });


    const vector<string> EnumTraits<MountByType>::names({
	"device", "uuid", "label", "id", "path"
    });


    Mountable::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
    }


    void
    Mountable::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);
    }


    MountPoint*
    Mountable::Impl::create_mount_point(const string& path)
    {
	Devicegraph* devicegraph = get_devicegraph();

	MountPoint* mount_point = MountPoint::create(devicegraph, path);

	User::create(devicegraph, get_device(), mount_point);

	mount_point->set_default_mount_by();
	mount_point->set_default_mount_options();

	return mount_point;
    }


    bool
    Mountable::Impl::has_mount_point() const
    {
	return num_children_of_type<const MountPoint>() > 0;
    }


    MountPoint*
    Mountable::Impl::get_mount_point()
    {
	vector<MountPoint*> tmp = get_children_of_type<MountPoint>();
	if (tmp.empty())
	    ST_THROW(Exception("no mount point"));

	return tmp.front();
    }


    const MountPoint*
    Mountable::Impl::get_mount_point() const
    {
	vector<const MountPoint*> tmp = get_children_of_type<const MountPoint>();
	if (tmp.empty())
	    ST_THROW(Exception("no mount point"));

	return tmp.front();
    }


    MountOpts
    Mountable::Impl::get_default_mount_options() const
    {
	return MountOpts();
    }


    FstabEntry*
    Mountable::Impl::find_etc_fstab_entry(EtcFstab& etc_fstab, const vector<string>& names) const
    {
	return etc_fstab.find_device(names);
    }


    const FstabEntry*
    Mountable::Impl::find_etc_fstab_entry(const EtcFstab& etc_fstab, const vector<string>& names) const
    {
	return etc_fstab.find_device(names);
    }


    Text
    Mountable::Impl::do_mount_text(const MountPoint* mount_point, Tense tense) const
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

	return sformat(text, get_mount_name().c_str(), mount_point->get_path().c_str());
    }


    void
    Mountable::Impl::do_mount(CommitData& commit_data, const MountPoint* mount_point) const
    {
	const Storage& storage = commit_data.actiongraph.get_storage();

	string real_mount_point = storage.get_impl().prepend_rootprefix(mount_point->get_path());
	if (access(real_mount_point.c_str(), R_OK ) != 0)
	{
	    createPath(real_mount_point);
	}

	string cmd_line = MOUNTBIN " -t " + toString(get_mount_type());
	if (!mount_point->get_mount_options().empty())
	    cmd_line += " -o " + quote(mount_point->get_impl().get_mount_options().format());
	cmd_line += " " + quote(get_mount_name()) + " " + quote(real_mount_point);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("mount failed"));

	if (mount_point->get_path() == "/")
	{
	    string path = get_storage()->prepend_rootprefix("/etc");
	    if (access(path.c_str(), R_OK) != 0)
		createPath(path);
	}
    }


    Text
    Mountable::Impl::do_umount_text(const MountPoint* mount_point, Tense tense) const
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

	return sformat(text, get_mount_name().c_str(), mount_point->get_path().c_str());
    }


    void
    Mountable::Impl::do_umount(CommitData& commit_data, const MountPoint* mount_point) const
    {
	const Storage& storage = commit_data.actiongraph.get_storage();

	string real_mount_point = storage.get_impl().prepend_rootprefix(mount_point->get_path());

	string cmd_line = UMOUNTBIN " " + quote(real_mount_point);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("umount failed"));
    }


    Text
    Mountable::Impl::do_add_to_etc_fstab_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Add mount point %1$s of %2$s to /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Adding mount point %1$s of %2$s to /etc/fstab"));

	return sformat(text, mount_point->get_path().c_str(), get_mount_name().c_str());
    }


    void
    Mountable::Impl::do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const
    {
	EtcFstab& etc_fstab = commit_data.get_etc_fstab();

	FstabEntry* entry = new FstabEntry();
	entry->set_device(get_mount_by_name());
	entry->set_mount_point(mount_point->get_path());
	entry->set_mount_opts(mount_point->get_impl().get_mount_options());
	entry->set_fs_type(get_mount_type());
	entry->set_fsck_pass(mount_point->get_passno());
	entry->set_dump_pass(mount_point->get_freq());

	etc_fstab.add(entry);
	etc_fstab.log_diff();
	etc_fstab.write();
    }


    Text
    Mountable::Impl::do_update_in_etc_fstab_text(const MountPoint* mount_point, const Device* lhs, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Update mount point %1$s of %2$s in /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Updating mount point %1$s of %2$s in /etc/fstab"));

	return sformat(text, mount_point->get_path().c_str(), get_mount_name().c_str());
    }


    void
    Mountable::Impl::do_update_in_etc_fstab(CommitData& commit_data, const Device* lhs, const MountPoint* mount_point) const
    {
	EtcFstab& etc_fstab = commit_data.get_etc_fstab();

	FstabEntry* entry = find_etc_fstab_entry(etc_fstab, { mount_point->get_impl().get_fstab_device_name() });
	if (entry)
	{
	    entry->set_device(get_mount_by_name());
	    entry->set_mount_point(mount_point->get_path());
	    entry->set_mount_opts(mount_point->get_impl().get_mount_options());
	    entry->set_fs_type(get_mount_type());
	    entry->set_fsck_pass(mount_point->get_passno());
	    entry->set_dump_pass(mount_point->get_freq());

	    etc_fstab.log_diff();
	    etc_fstab.write();
	}
    }


    Text
    Mountable::Impl::do_remove_from_etc_fstab_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Remove mount point %1$s of %2$s from /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda1)
			   _("Removing mount point %1$s of %2$s from /etc/fstab"));

	return sformat(text, mount_point->get_path().c_str(), get_mount_name().c_str());
    }


    void
    Mountable::Impl::do_remove_from_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const
    {
	EtcFstab& etc_fstab = commit_data.get_etc_fstab();

	FstabEntry* entry = find_etc_fstab_entry(etc_fstab, { mount_point->get_impl().get_fstab_device_name() });
	if (entry)
	{
	    etc_fstab.remove(entry);
	    etc_fstab.log_diff();
	    etc_fstab.write();
	}
    }


    EnsureMounted::EnsureMounted(const Mountable* mountable, bool read_only)
	: mountable(mountable), read_only(read_only), tmp_mount()
    {
	y2mil("EnsureMounted " << *mountable);

	bool need_mount = false;

	if (mountable->get_impl().get_devicegraph()->get_impl().is_probed())
	{
	    // Called on probed devicegraph.

	    need_mount = !mountable_has_active_mount_point();
	}
	else
	{
	    // Not called on probed devicegraph.

	    if (is_blk_filesystem(mountable))
	    {
		// The names of BlkDevices may have changed so redirect to the
		// Mountable in the probed devicegraph.

		mountable = redirect_to_probed(mountable);
		need_mount = !mountable_has_active_mount_point();
	    }
	    else
	    {
		// Nfs can be temporarily mounted on any devicegraph. But the
		// list of mountpoints is only useful for the probed
		// devicegraph, so redirect if the mountable exists there to
		// avoid unnecessary temporary mounts.

		if (mountable->exists_in_probed())
		{
		    mountable = redirect_to_probed(mountable);
		    need_mount = !mountable_has_active_mount_point();
		}
		else
		{
		    need_mount = true;
		}
	    }
	}

	y2mil("EnsureMounted need_mount:" << need_mount);

	if (!need_mount)
	    return;

	const Storage* storage = mountable->get_impl().get_storage();

	if (is_blk_filesystem(mountable))
	{
	    const BlkFilesystem* blk_filesystem = to_blk_filesystem(mountable);
	    for (const BlkDevice* blk_device : blk_filesystem->get_blk_devices())
		blk_device->get_impl().wait_for_device();
	}

        tmp_mount.reset(new TmpMount(storage->get_impl().get_tmp_dir().get_fullname(),
				     "tmp-mount-XXXXXX", mountable->get_impl().get_mount_name(),
				     read_only, mountable->get_impl().get_mount_options()));
    }


    string
    EnsureMounted::get_any_mount_point() const
    {
	if (tmp_mount)
	    return tmp_mount->get_fullname();
	else
	    return mountable->get_mount_point()->get_path();
    }


    bool
    EnsureMounted::mountable_has_active_mount_point() const
    {
	if (!mountable->has_mount_point())
	    return false;

	return mountable->get_mount_point()->is_active();
    }

}
