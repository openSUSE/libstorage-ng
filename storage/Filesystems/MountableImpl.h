/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_MOUNTABLE_IMPL_H
#define STORAGE_MOUNTABLE_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/CDgD.h"
#include "storage/Utils/FileUtils.h"
#include "storage/Filesystems/Mountable.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/EtcFstab.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;

    class SystemInfo;
    class FstabAnchor;


    template <> struct DeviceTraits<Mountable> { static const char* classname; };

    template <> struct EnumTraits<FsType> { static const vector<string> names; };

    template <> struct EnumTraits<MountByType> { static const vector<string> names; };


    /**
     * Checks whether the mount_by type references the blk device.
     */
    bool mount_by_references_blk_device(MountByType mount_by);


    /**
     * Checks whether the mount_by type references the filesystem, either by UUID or LABEL.
     */
    bool mount_by_references_filesystem(MountByType mount_by);


    // abstract class

    class Mountable::Impl : public Device::Impl
    {
    public:

	virtual bool supports_mount() const { return true; }

	MountPoint* create_mount_point(const string& path);

	bool has_mount_point() const;

	MountPoint* get_mount_point();
	const MountPoint* get_mount_point() const;

	void remove_mount_point();

	virtual bool has_filesystem() const;

	virtual Filesystem* get_filesystem();
	virtual const Filesystem* get_filesystem() const;

	virtual MountByType get_default_mount_by() const = 0;

	virtual MountOpts default_mount_options() const;

	/**
	 * A name for the mountable in messages. E.g. for
	 * BlkFilesystems a list of the underlying blk devices
	 * including sizes and for Nfs the server plus the path.
	 */
	virtual Text get_message_name() const = 0;

	/**
	 * A name useable for mounting the mountable. E.g. for BlkFilesystems
	 * the name of a device and for Nfs the server plus the path.
	 *
	 * For messages get_message_name() must be used.
	 */
	virtual string get_mount_name() const = 0;

	/**
	 * A name useable for mounting the mountable. E.g. for BlkFilesystems
	 * the name of a device and for Nfs the server plus the path. Used for
	 * entries in /etc/fstab.
	 */
	virtual string get_mount_by_name(const MountPoint* mount_point) const = 0;

	virtual FsType get_default_mount_type() const = 0;

	virtual vector<string> get_mount_options() const { return vector<string>(); }

	/**
	 * Somehow waits for all underlying/parent devices to be present,
	 * e.g. run "udevadm settle" and check device nodes.
	 */
	virtual void wait_for_devices() const { }

	virtual Mountable* get_non_impl() override { return to_mountable(Device::Impl::get_non_impl()); }
	virtual const Mountable* get_non_impl() const override { return to_mountable(Device::Impl::get_non_impl()); }

	/**
	 * Add mount action for all active mount points. Likely always used on RHS.
	 */
	virtual void insert_mount_action(vector<Action::Base*>& actions) const;

	/**
	 * Add unmount action for all active mount points. Likely always used on LHS.
	 */
	virtual void insert_unmount_action(vector<Action::Base*>& actions) const;

	virtual void probe_pass_2b(Prober& prober);

	/**
	 * Find the fstab entry for the Mountable. Normally just looks for the
	 * device but for Btrfs and BtrfsSubvolume also the subvol option has
	 * to fit (see filter_etc_fstab()).
	 *
	 * Used for probe.
	 */
	vector<ExtendedFstabEntry> find_etc_fstab_entries(SystemInfo& system_info) const;

	/**
	 * Unfiltered version of find_etc_fstab_entries().
	 *
	 * Used for probe.
	 */
	virtual vector<ExtendedFstabEntry> find_etc_fstab_entries_unfiltered(SystemInfo& system_info) const;

	/**
	 * Find the fstab entry for the Mountable. Normally just looks for the
	 * device but for Btrfs and BtrfsSubvolume also the subvol option has
	 * to fit (see filter_etc_fstab()).
	 *
	 * Used for commit.
	 */
	vector<FstabEntry*> find_etc_fstab_entries(EtcFstab& etc_fstab, const FstabAnchor& fstab_anchor) const;

	/**
	 * Unfiltered version of find_etc_fstab_entries().
	 *
	 * Used for commit.
	 */
	virtual vector<FstabEntry*> find_etc_fstab_entries_unfiltered(EtcFstab& etc_fstab,
								      const FstabAnchor& fstab_anchor) const;

	/**
	 * Predicate for filtering. So far only needed for btrfs where
	 * entries are filtered based on the subvol option.
	 *
	 * Used for probe and commit.
	 */
	virtual bool predicate_etc_fstab(const FstabEntry* fstab_entry) const { return true; }

	/**
	 * Find the fstab entry for the Mountable in /proc/mounts. Normally
	 * just looks for the device but for Btrfs and BtrfsSubvolume also the
	 * subvol option has to fit (see filter_proc_mounts()).
	 *
	 * Used for probe.
	 */
	vector<ExtendedFstabEntry> find_proc_mounts_entries(SystemInfo& system_info) const;

	/**
	 * Find the fstab entry for the Mountable in /proc/mounts. Normally
	 * just looks for the device but for Btrfs and BtrfsSubvolume also the
	 * subvol option has to fit.
	 *
	 * Used for probe.
	 */
	virtual vector<ExtendedFstabEntry> find_proc_mounts_entries_unfiltered(SystemInfo& system_info) const = 0;

	/**
	 * Predicate for filtering. So far only needed for btrfs where
	 * entries are filtered based on the subvol option.
	 *
	 * Used for probe.
	 */
	virtual bool predicate_proc_mounts(const FstabEntry* fstab_entry) const { return true; }

	virtual void do_pre_mount() const {}

	virtual Text do_mount_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_mount(CommitData& commit_data, const CommitOptions& commit_options, MountPoint* mount_point) const;

	virtual Text do_unmount_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_unmount(CommitData& commit_data, MountPoint* mount_point) const;

	virtual Text do_add_to_etc_fstab_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const;

	virtual Text do_update_in_etc_fstab_text(const MountPoint* mount_point, const Device* lhs, Tense tense) const;
	virtual void do_update_in_etc_fstab(CommitData& commit_data, const Device* lhs, const MountPoint* mount_point) const;

	virtual Text do_remove_from_etc_fstab_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_remove_from_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const;

	/**
	 * Checks if the mount point of the mountable is active at
	 * present. Does system lookups via the system_info.
	 */
	virtual bool is_active_at_present(SystemInfo& system_info, const MountPoint* mount_point) const;

	virtual void immediate_activate(MountPoint* mount_point, bool force_rw = false) const;
	virtual void immediate_deactivate(MountPoint* mount_point) const;

    protected:

	Impl()
	    : Device::Impl()
	    {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    };


    /**
     * Class to ensure that a mountable is mounted. If mounting is
     * required the constructor does mounting and the destructor does
     * unmounting.
     */
    class EnsureMounted : boost::noncopyable
    {

    public:

	/**
	 * Ensures that the blk mountable is mounted somewhere.
	 *
	 * The flag read_only is not enforced.
	 *
	 * BlkFilesystems must exist in the system
	 * devicegraph. Otherwise an exception in thrown.
	 *
	 * If mounting is required and fails an exception is thrown.
	 */
	EnsureMounted(const Mountable* mountable, bool read_only = true);

	~EnsureMounted();

	/**
	 * Returns any mountpoint of the mountable.
	 */
	string get_any_mount_point() const;

    private:

	/**
	 * Check whether mounting is required.
	 *
	 * May also redirect the mountable to the system devicegraph.
	 */
	bool mount_needed();

	/**
	 * Mount the mountable.
	 */
	void do_mount(bool read_only);

	bool mountable_has_active_mount_point() const;

	const Mountable* mountable;

	unique_ptr<TmpMount> tmp_mount;

    };

}

#endif
