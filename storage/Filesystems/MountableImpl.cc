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
#include "storage/Devices/BlkDeviceImpl.h"
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
	: Device::Impl(node), mountpoints({}), mount_by(MountByType::DEVICE)
    {
	string tmp;

	getChildValue(node, "mountpoint", mountpoints);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "mount-opts", tmp))
	    mount_opts.parse(tmp);
    }


    void
    Mountable::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "mountpoint", mountpoints);

	setChildValueIf(node, "mount-by", toString(mount_by), mount_by != MountByType::DEVICE);

	if (!mount_opts.empty())
	    setChildValue(node, "mount-opts", mount_opts.format());
    }


    void
    Mountable::Impl::set_mountpoints(const vector<string>& mountpoints)
    {
	Impl::mountpoints = mountpoints;
    }


    void
    Mountable::Impl::add_mountpoint(const string& mountpoint)
    {
	Impl::mountpoints.push_back(mountpoint);
    }


    void
    Mountable::Impl::set_mount_by(MountByType mount_by)
    {
	Impl::mount_by = mount_by;
    }


    void
    Mountable::Impl::set_mount_opts(const MountOpts & new_mount_opts)
    {
	Impl::mount_opts = new_mount_opts;
    }


    void
    Mountable::Impl::set_mount_opts(const vector<string>& new_mount_opts)
    {
	Impl::mount_opts.set_opts(new_mount_opts);
    }


    bool
    Mountable::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return mountpoints == rhs.mountpoints && mount_by == rhs.mount_by &&
	    mount_opts.get_opts() == rhs.mount_opts.get_opts();
    }


    void
    Mountable::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "mountpoints", mountpoints, rhs.mountpoints);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

	storage::log_diff(log, "mount-opts", mount_opts.get_opts(), rhs.mount_opts.get_opts());
    }


    void
    Mountable::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	if (!mountpoints.empty())
	    out << " mountpoints:" << mountpoints;

	if (!mount_opts.empty())
	    out << " mount-opts:" << mount_opts.format();
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
    Mountable::Impl::do_mount_text(const string& mountpoint, Tense tense) const
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

	return sformat(text, get_mount_name().c_str(), mountpoint.c_str());
    }


    void
    Mountable::Impl::do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	string real_mountpoint = storage.get_impl().prepend_rootprefix(mountpoint);
	if (access(real_mountpoint.c_str(), R_OK ) != 0)
	{
	    createPath(real_mountpoint);
	}

	string cmd_line = MOUNTBIN " -t " + toString(get_mount_type());
	if (!mount_opts.empty())
	    cmd_line += " -o " + boost::join(mount_opts.get_opts(), ",");
	cmd_line += " " + quote(get_mount_name()) + " " + quote(real_mountpoint);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("mount failed"));
    }


    Text
    Mountable::Impl::do_umount_text(const string& mountpoint, Tense tense) const
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

	return sformat(text, get_mount_name().c_str(), mountpoint.c_str());
    }


    void
    Mountable::Impl::do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
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
    Mountable::Impl::do_add_to_etc_fstab_text(const string& mountpoint, Tense tense) const
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

	return sformat(text, mountpoint.c_str(), get_mount_name().c_str());
    }


    void
    Mountable::Impl::do_add_to_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab;
        fstab.read(storage.get_impl().prepend_rootprefix(ETC_FSTAB));	// TODO pass as parameter

        FstabEntry * entry = new FstabEntry();
        entry->set_device(get_mount_by_name());
        entry->set_mount_point(mountpoint);
        entry->set_mount_opts(get_mount_opts());
	entry->set_fs_type(get_mount_type());

        // TODO: entry->set_fsck_pass( ?? );

        fstab.add(entry);
        fstab.log_diff();
        fstab.write();
    }


    Text
    Mountable::Impl::do_remove_from_etc_fstab_text(const string& mountpoint, Tense tense) const
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

	return sformat(text, mountpoint.c_str(), get_mount_name().c_str());
    }


    void
    Mountable::Impl::do_remove_from_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab;
        fstab.read(storage.get_impl().prepend_rootprefix(ETC_FSTAB));	// TODO pass as parameter

	FstabEntry* entry = find_etc_fstab_entry(fstab, { get_fstab_device_name() });
	if (entry)
        {
            fstab.remove( entry );
            fstab.log_diff();
            fstab.write();
        }
    }


    namespace Action
    {

	Text
	Mount::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Mountable* mountable = to_mountable(get_device(actiongraph, RHS));
	    return mountable->get_impl().do_mount_text(mountpoint, tense);
	}


	void
	Mount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Mountable* mountable = to_mountable(get_device(actiongraph, RHS));
	    mountable->get_impl().do_mount(actiongraph, mountpoint);
	}


	Text
	Umount::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Mountable* mountable = to_mountable(get_device(actiongraph, LHS));
	    return mountable->get_impl().do_umount_text(mountpoint, tense);
	}


	void
	Umount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Mountable* mountable = to_mountable(get_device(actiongraph, LHS));
	    mountable->get_impl().do_umount(actiongraph, mountpoint);
	}


	Text
	AddToEtcFstab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Mountable* mountable = to_mountable(get_device(actiongraph, RHS));
	    return mountable->get_impl().do_add_to_etc_fstab_text(mountpoint, tense);
	}


	void
	AddToEtcFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Mountable* mountable = to_mountable(get_device(actiongraph, RHS));
	    mountable->get_impl().do_add_to_etc_fstab(actiongraph, mountpoint);
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
	    const Mountable* mountable = to_mountable(get_device(actiongraph, LHS));
	    return mountable->get_impl().do_remove_from_etc_fstab_text(mountpoint, tense);
	}


	void
	RemoveFromEtcFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Mountable* mountable = to_mountable(get_device(actiongraph, LHS));
	    mountable->get_impl().do_remove_from_etc_fstab(actiongraph, mountpoint);
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

	    need_mount = mountable->get_mountpoints().empty();
	}
	else
	{
	    // Not called on probed devicegraph.

	    if (is_blk_filesystem(mountable))
	    {
		// The names of BlkDevices may have changed so redirect to the
		// Mountable in the probed devicegraph.

		mountable = redirect_to_probed(mountable);
		need_mount = mountable->get_mountpoints().empty();
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
		    need_mount = mountable->get_mountpoints().empty();
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
    EnsureMounted::get_any_mountpoint() const
    {
	if (tmp_mount)
	    return tmp_mount->get_fullname();
	else
	    return mountable->get_mountpoints().front();
    }

}
