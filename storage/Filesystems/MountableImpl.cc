/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2026] SUSE LLC
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


#include <algorithm>
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
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/StorageImpl.h"
#include "storage/Redirect.h"
#include "storage/Utils/Format.h"
#include "storage/Prober.h"
#include "storage/Actions/MountImpl.h"
#include "storage/Actions/UnmountImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Mountable>::classname = "Mountable";


    // Strings must match /etc/fstab and output of blkid - hopefully they are
    // always the same. Some names are not used in the output of blkid,
    // e.g. unknown, auto, tmpfs and ntfs-3g.
    const vector<string> EnumTraits<FsType>::names({
	"unknown", "auto", "reiserfs", "ext2", "ext3", "ext4", "btrfs", "vfat", "xfs", "jfs", "hfs",
	"ntfs", "swap", "hfsplus", "nfs", "nfs4", "tmpfs", "iso9660", "udf", "nilfs2", "minix",
	"ntfs-3g", "f2fs", "exfat", "BitLocker", "vboxsf", "bcachefs", "squashfs", "erofs"
    });


    const vector<string> EnumTraits<MountByType>::names({
	"device", "uuid", "label", "id", "path", "partuuid", "partlabel"
    });


    bool
    mount_by_references_blk_device(MountByType mount_by)
    {
	return mount_by == MountByType::DEVICE || mount_by == MountByType::ID ||
	    mount_by == MountByType::PATH || mount_by == MountByType::PARTUUID ||
	    mount_by == MountByType::PARTLABEL;
    }


    bool
    mount_by_references_filesystem(MountByType mount_by)
    {
	return mount_by == MountByType::UUID || mount_by == MountByType::LABEL;
    }


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

	User::create(devicegraph, get_non_impl(), mount_point);

	mount_point->set_default_mount_by();
	mount_point->get_impl().set_default_mount_type();
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


    void
    Mountable::Impl::remove_mount_point()
    {
	get_devicegraph()->remove_device(get_mount_point());
    }


    bool
    Mountable::Impl::has_filesystem() const
    {
	return true;
    }


    Filesystem*
    Mountable::Impl::get_filesystem()
    {
	return to_filesystem(get_non_impl());
    }


    const Filesystem*
    Mountable::Impl::get_filesystem() const
    {
	return to_filesystem(get_non_impl());
    }


    MountOpts
    Mountable::Impl::default_mount_options() const
    {
	return MountOpts();
    }


    void
    Mountable::Impl::insert_mount_action(vector<shared_ptr<Action::Base>>& actions) const
    {
	if (has_mount_point())
	{
	    const MountPoint* mount_point = get_mount_point();
	    if (mount_point->is_active())
		actions.push_back(make_shared<Action::Mount>(mount_point->get_sid()));
	}
    }


    void
    Mountable::Impl::insert_unmount_action(vector<shared_ptr<Action::Base>>& actions) const
    {
	if (has_mount_point())
	{
	    const MountPoint* mount_point = get_mount_point();
	    if (mount_point->is_active())
		actions.push_back(make_shared<Action::Unmount>(mount_point->get_sid()));
	}
    }


    void
    Mountable::Impl::probe_pass_2b(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();

	vector<ExtendedFstabEntry> fstab_entries = find_etc_fstab_entries(system_info);
	vector<ExtendedFstabEntry> mount_entries = find_proc_mounts_entries(system_info);

	// The code here works only with one mount point per
	// mountable. Anything else is not supported since rejected by the
	// product owner.

	vector<JointEntry> joint_entries = join_entries(fstab_entries, mount_entries);

	if (!joint_entries.empty())
	{
	    if (joint_entries.size() > 1)
	    {
		y2war("more than one mount point for " << (*this->get_non_impl()));

		for (const JointEntry& tmp : joint_entries)
		    y2war(tmp.mount_point_path.fullpath(get_storage()->get_rootprefix()));
	    }

	    // Selecting the most reasonable mount point:
	    //
	    // * shortest active mount point from the fstab or
	    // * shortest active mount point or
	    // * shortest non-active mount point

	    vector<const JointEntry*> filtered_entries;

	    for (const JointEntry& entry : joint_entries)
		filtered_entries.push_back(&entry);

	    // Check if there is any active entry
	    vector<const JointEntry*>::iterator active_entry = find_if(filtered_entries.begin(), filtered_entries.end(),
		[](const JointEntry* entry) {
		    return entry->is_active();
		}
	    );

	    if (active_entry != filtered_entries.end())
	    {
		// There are active entries, so discarding no active ones
		filtered_entries.erase(
		    remove_if(filtered_entries.begin(), filtered_entries.end(),
			[](const JointEntry* entry) { return !entry->is_active(); }
		    ),
		    filtered_entries.end()
		);

		// Check if any active entry is in /etc/fstab
		vector<const JointEntry*>::iterator fstab_entry = find_if(filtered_entries.begin(), filtered_entries.end(),
		    [](const JointEntry* entry) {
			return entry->is_in_etc_fstab();
		    }
		);

		if (fstab_entry != filtered_entries.end())
		{
		    // There are active entries in /etc/fstab, so discarding the rest
		    filtered_entries.erase(
			remove_if(filtered_entries.begin(), filtered_entries.end(),
			    [](const JointEntry* entry) { return !entry->is_in_etc_fstab(); }
			),
			filtered_entries.end()
		    );
		}
	    }

	    // Select the shortest path to avoid problems during upgrade (bsc#1118865).
	    const JointEntry* entry = *min_element(filtered_entries.begin(), filtered_entries.end(), compare_by_size);

	    entry->add_to(get_non_impl());
	}
    }


    vector<ExtendedFstabEntry>
    Mountable::Impl::find_etc_fstab_entries_unfiltered(SystemInfo::Impl& system_info) const
    {
	return {};
    }


    vector<FstabEntry*>
    Mountable::Impl::find_etc_fstab_entries_unfiltered(EtcFstab& etc_fstab, const FstabAnchor& fstab_anchor) const
    {
	return etc_fstab.find_all_by_spec_and_mount_point(fstab_anchor.spec, fstab_anchor.mount_point);
    }


    vector<ExtendedFstabEntry>
    Mountable::Impl::find_etc_fstab_entries(SystemInfo::Impl& system_info) const
    {
	vector<ExtendedFstabEntry> ret;

	for (const ExtendedFstabEntry& extended_fstab_entry : find_etc_fstab_entries_unfiltered(system_info))
	{
	    if (predicate_etc_fstab(extended_fstab_entry.fstab_entry))
		ret.push_back(extended_fstab_entry);
	}

	return ret;
    }


    vector<FstabEntry*>
    Mountable::Impl::find_etc_fstab_entries(EtcFstab& etc_fstab, const FstabAnchor& fstab_anchor) const
    {
	vector<FstabEntry*> ret;

	for (FstabEntry* fstab_entry : find_etc_fstab_entries_unfiltered(etc_fstab, fstab_anchor))
	{
	    if (predicate_etc_fstab(fstab_entry))
		ret.push_back(fstab_entry);
	}

	return ret;
    }


    vector<ExtendedFstabEntry>
    Mountable::Impl::find_proc_mounts_entries(SystemInfo::Impl& system_info) const
    {
	vector<ExtendedFstabEntry> ret;

	for (const ExtendedFstabEntry& extended_fstab_entry : find_proc_mounts_entries_unfiltered(system_info))
	{
	    if (predicate_proc_mounts(extended_fstab_entry.fstab_entry))
		ret.push_back(extended_fstab_entry);
	}

	return ret;
    }


    uf_t
    Mountable::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	uf_t ret = Device::Impl::used_features(used_features_dependency_type);

	if (used_features_dependency_type == UsedFeaturesDependencyType::SUGGESTED)
	    ret |= used_features_pure();

	return ret;
    }


    Text
    Mountable::Impl::do_mount_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB)),
			   // %2$s is replaced by the mount point (e.g. /home)
			   _("Mount %1$s at %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB)),
			   // %2$s is replaced by the mount point (e.g. /home)
			   _("Mounting %1$s at %2$s"));

	return sformat(text, get_message_name(), mount_point->get_path());
    }


    void
    Mountable::Impl::do_mount(CommitData& commit_data, const CommitOptions& commit_options,
			      MountPoint* mount_point) const
    {
	immediate_activate(mount_point, commit_options.force_rw);

	if (mount_point->get_path() == "/")
	{
	    string path = get_storage()->prepend_rootprefix(ETC_DIR);
	    if (access(path.c_str(), R_OK) != 0)
		createPath(path);
	}
    }


    Text
    Mountable::Impl::do_unmount_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB)),
			   // %2$s is replaced by the mount point (e.g. /home)
			   _("Unmount %1$s at %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB)),
			   // %2$s is replaced by the mount point (e.g. /home)
			   _("Unmounting %1$s at %2$s"));

	return sformat(text, get_message_name(), mount_point->get_path());
    }


    void
    Mountable::Impl::do_unmount(CommitData& commit_data, MountPoint* mount_point) const
    {
	immediate_deactivate(mount_point);
    }


    Text
    Mountable::Impl::do_add_to_etc_fstab_text(const MountPoint* mount_point, Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Add mount point %1$s of %2$s to /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Adding mount point %1$s of %2$s to /etc/fstab"));

	return sformat(text, mount_point->get_path(), get_message_name());
    }


    void
    Mountable::Impl::do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const
    {
	EtcFstab& etc_fstab = commit_data.get_etc_fstab();

	FstabEntry* entry = new FstabEntry();
	entry->set_spec(get_mount_by_name(mount_point));
	entry->set_mount_point(mount_point->get_path());
	entry->set_mount_opts(mount_point->get_impl().get_mount_options());
	entry->set_fs_type(mount_point->get_mount_type());
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
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Update mount point %1$s of %2$s in /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Updating mount point %1$s of %2$s in /etc/fstab"));

	return sformat(text, mount_point->get_path(), get_message_name());
    }


    void
    Mountable::Impl::do_update_in_etc_fstab(CommitData& commit_data, const Device* lhs, const MountPoint* mount_point) const
    {
	EtcFstab& etc_fstab = commit_data.get_etc_fstab();

	const FstabAnchor& fstab_anchor = mount_point->get_impl().get_fstab_anchor();

	for (FstabEntry* entry : find_etc_fstab_entries(etc_fstab, fstab_anchor))
	{
	    entry->set_spec(get_mount_by_name(mount_point));
	    entry->set_mount_point(mount_point->get_path());
	    entry->set_mount_opts(mount_point->get_impl().get_mount_options());
	    entry->set_fs_type(mount_point->get_mount_type());
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
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Remove mount point %1$s of %2$s from /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the mount point (e.g. /home),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Removing mount point %1$s of %2$s from /etc/fstab"));

	return sformat(text, mount_point->get_path(), get_message_name());
    }


    void
    Mountable::Impl::do_remove_from_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const
    {
	EtcFstab& etc_fstab = commit_data.get_etc_fstab();

	const FstabAnchor& fstab_anchor = mount_point->get_impl().get_fstab_anchor();

	for (FstabEntry* entry : find_etc_fstab_entries(etc_fstab, fstab_anchor))
	{
	    etc_fstab.remove(entry);
	    etc_fstab.log_diff();
	    etc_fstab.write();
	}
    }


    bool
    Mountable::Impl::is_active_at_present(SystemInfo::Impl& system_info, const MountPoint* mount_point) const
    {
	y2mil("active check begin");

	vector<ExtendedFstabEntry> mount_entries = find_proc_mounts_entries(system_info);

	y2mil("active check end");

	y2mil("path:" << mount_point->get_path() << " active:" << !mount_entries.empty());

	return !mount_entries.empty();
    }


    void
    Mountable::Impl::immediate_activate(MountPoint* mount_point, bool force_rw) const
    {
	string real_mount_point = mount_point->get_impl().get_rootprefixed_path();

	if (access(real_mount_point.c_str(), R_OK) != 0)
	{
	    createPath(real_mount_point);
	}

	do_pre_mount();
	wait_for_devices();

	SystemCmd::Args cmd_args = { MOUNT_BIN, "-t", toString(mount_point->get_mount_type()) };

	MountOpts mount_opts = mount_point->get_impl().get_mount_options();
	if (force_rw)
	    mount_opts.remove("ro");
	if (!mount_opts.empty())
	    cmd_args << "-o"  << mount_opts.format();

	cmd_args << get_mount_name() << real_mount_point;

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);

	if (mount_point->exists_in_system())
	    redirect_to_system(mount_point)->set_active(true);
    }


    void
    Mountable::Impl::immediate_deactivate(MountPoint* mount_point) const
    {
	string real_mount_point = mount_point->get_impl().get_rootprefixed_path();

	SystemCmd::Args cmd_args = { UMOUNT_BIN, real_mount_point };

	try
	{
	    SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    SystemInfo::Impl system_info;

	    if (is_active_at_present(system_info, mount_point))
		ST_RETHROW(exception);

	    y2mil("ignoring umount failure since mount point seems already inactive");
	}

	if (mount_point->exists_in_system())
	    redirect_to_system(mount_point)->set_active(false);
    }


    EnsureMounted::EnsureMounted(const Mountable* mountable, bool read_only)
	: mountable(mountable), tmp_mount()
    {
	y2mil("EnsureMounted " << *mountable);

	if (!mountable->supports_mount())
	{
	    ST_THROW(Exception("mount not supported"));
	}

	if (mount_needed())
	{
	    y2mil("mount is needed");

	    do_mount(read_only);
	}
    }


    EnsureMounted::~EnsureMounted()
    {
	y2mil("~EnsureMounted " << *mountable);
    }


    bool
    EnsureMounted::mount_needed()
    {
	if (mountable->get_impl().get_devicegraph()->get_impl().is_system())
	{
	    // Called on system devicegraph.

	    return !mountable_has_active_mount_point();
	}
	else
	{
	    // Not called on system devicegraph.

	    if (is_blk_filesystem(mountable))
	    {
		// The names of BlkDevices may have changed so redirect to the
		// Mountable in the system devicegraph.

		mountable = redirect_to_system(mountable);
		return !mountable_has_active_mount_point();
	    }
	    else
	    {
		// Nfs can be temporarily mounted on any devicegraph. But the
		// list of mountpoints is only useful for the system
		// devicegraph, so redirect if the mountable exists there to
		// avoid unnecessary temporary mounts.

		if (mountable->exists_in_system())
		{
		    mountable = redirect_to_system(mountable);
		    return !mountable_has_active_mount_point();
		}
		else
		{
		    return true;
		}
	    }
	}
    }


    void
    EnsureMounted::do_mount(bool read_only)
    {
	const Storage* storage = mountable->get_impl().get_storage();

	mountable->get_impl().do_pre_mount();
	mountable->get_impl().wait_for_devices();

	tmp_mount = make_unique<TmpMount>(storage->get_impl().get_tmp_dir().get_fullname(),
					  "tmp-mount-XXXXXX", mountable->get_impl().get_mount_name(),
					  read_only, mountable->get_impl().get_mount_options());
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
