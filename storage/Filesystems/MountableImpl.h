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


    template <> struct DeviceTraits<Mountable> { static const char* classname; };

    template <> struct EnumTraits<FsType> { static const vector<string> names; };

    template <> struct EnumTraits<MountByType> { static const vector<string> names; };


    // abstract class

    class Mountable::Impl : public Device::Impl
    {
    public:

	MountPoint* create_mount_point(const string& path);

	bool has_mount_point() const;

	MountPoint* get_mount_point();
	const MountPoint* get_mount_point() const;

	virtual bool has_filesystem() const;

	virtual Filesystem* get_filesystem();
	virtual const Filesystem* get_filesystem() const;

	virtual MountByType get_default_mount_by() const = 0;

	virtual MountOpts get_default_mount_options() const;

	/**
	 * A name useable for mounting the mountable. E.g. for BlkFilesystems
	 * the name of a device and for Nfs the server plus the path. Used for
	 * messages and mount command.
	 */
	virtual string get_mount_name() const = 0;

	/**
	 * A name useable for mounting the mountable. E.g. for BlkFilesystems
	 * the name of a device and for Nfs the server plus the path. Used for
	 * entries in /etc/fstab.
	 */
	virtual string get_mount_by_name(MountByType mount_by_type) const = 0;

	virtual FsType get_mount_type() const = 0;

	virtual vector<string> get_mount_options() const { return vector<string>(); }

	virtual Mountable* get_non_impl() override { return to_mountable(Device::Impl::get_non_impl()); }
	virtual const Mountable* get_non_impl() const override { return to_mountable(Device::Impl::get_non_impl()); }

	/**
	 * Find the fstab entry for the Mountable. Normally just looks for the
	 * device but for Btrfs and BtrfsSubvolume also the subvol option has
	 * to fit.
	 *
	 * During probing names should be the device aliases, during commit
	 * actions only the 'fstab device name'.
	 */
	virtual FstabEntry* find_etc_fstab_entry(EtcFstab& etc_fstab, const vector<string>& names) const;
	virtual const FstabEntry* find_etc_fstab_entry(const EtcFstab& etc_fstab, const vector<string>& names) const;

	virtual Text do_mount_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_mount(CommitData& commit_data, const MountPoint* mount_point) const;

	virtual Text do_umount_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_umount(CommitData& commit_data, const MountPoint* mount_point) const;

	virtual Text do_add_to_etc_fstab_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const;

	virtual Text do_update_in_etc_fstab_text(const MountPoint* mount_point, const Device* lhs, Tense tense) const;
	virtual void do_update_in_etc_fstab(CommitData& commit_data, const Device* lhs, const MountPoint* mount_point) const;

	virtual Text do_remove_from_etc_fstab_text(const MountPoint* mount_point, Tense tense) const;
	virtual void do_remove_from_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const;

    protected:

	Impl()
	    : Device::Impl()
	    {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    };


    class EnsureMounted : boost::noncopyable
    {

    public:

	/**
	 * Ensures that the blk mountable is mounted somewhere.
	 *
	 * The mode is not enforced.
	 */
	EnsureMounted(const Mountable* mountable, bool read_only = true);

	/**
	 * Returns any mountpoint of the mountable.
	 */
	string get_any_mount_point() const;

    private:

	bool mountable_has_active_mount_point() const;

	const Mountable* mountable;

	unique_ptr<TmpMount> tmp_mount;

    };

}

#endif
