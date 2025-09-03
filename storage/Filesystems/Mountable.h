/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2022] SUSE LLC
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


#ifndef STORAGE_MOUNTABLE_H
#define STORAGE_MOUNTABLE_H


#include <vector>

#include "storage/Devices/Device.h"
#include "storage/Utils/Swig.h"


namespace storage
{

    class MountPoint;
    class Filesystem;


    enum class FsType {
	UNKNOWN, AUTO, REISERFS, EXT2, EXT3, EXT4, BTRFS, VFAT, XFS, JFS, HFS, NTFS,
	SWAP, HFSPLUS, NFS, NFS4, TMPFS, ISO9660, UDF, NILFS2, MINIX, NTFS3G, F2FS,
	EXFAT, BITLOCKER, VBOXSF, BCACHEFS
    };


    /**
     * Convert the FsType fs_type to a string.
     *
     * @see FsType
     */
    std::string get_fs_type_name(FsType fs_type);


    /**
     * The key by which the mount program identifies a mountable. Some types
     * address the filesystem while others address the underlying device.
     */
    enum class MountByType {

	/**
	 * The kernel device name or a link in /dev (but not in
	 * /dev/disk). For NFS the server and path.
	 */
	DEVICE,

	/**
	 * Filesystem UUID.
	 */
	UUID,

	/**
	 * Filesystem label.
	 */
	LABEL,

	/**
	 * One of the links in /dev/disk/by-id.
	 */
	ID,

	/**
	 * One of the links in /dev/disk/by-path.
	 */
	PATH,

	/**
	 * Partition UUID, only for partitions on GPT. Experimental.
	 */
	PARTUUID,

	/**
	 * Partition label, only for partitions on GPT. Experimental.
	 */
	PARTLABEL,

    };


    /**
     * Convert the MountByType mount_by_type to a string.
     *
     * @see MountByType
     */
    std::string get_mount_by_name(MountByType mount_by_type);


    // abstract class

    class Mountable : public Device
    {
    public:

	/**
	 * Get all Mountables.
	 */
	static std::vector<Mountable*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Mountable*> get_all(const Devicegraph* devicegraph);

	/**
	 * Checks whether the library supports mounting the
	 * mountable. So far this is always the case except for
	 * Bitlocker.
	 *
	 * Note: Mounting may still fail if the system does not
	 * support it, e.g. due to missing kernel modules or missing
	 * user space tools.
	 */
	bool supports_mount() const;

	/**
	 * Checks whether the path is valid. For FsType::UNKNOWN and FsType::AUTO the
	 * result is always false.
	 */
	static bool is_valid_path(FsType fs_type, const std::string& path);

	/**
	 * Create a new mount point for the mountable with path. It will also
	 * set the default mount-by method to the storage default mount-by
	 * method and the default mount options.
	 */
	MountPoint* create_mount_point(const std::string& path);

	/**
	 * Checks whether the mountable has a mount point.
	 */
	bool has_mount_point() const;

	/**
	 * Returns the mount point of the mountable.
	 */
	MountPoint* get_mount_point();

	/**
	 * @copydoc get_mount_point()
	 */
	const MountPoint* get_mount_point() const;

	/**
	 * Removes the mount point from the mountable.
	 *
	 * @throw Exception
	 */
	void remove_mount_point();

	/**
	 * Checks whether the mountable has a filesystem. Currently always true.
	 */
	bool has_filesystem() const;

	/**
	 * Return the filesystem of the mountable. Normally this is the same
	 * object but for btrfs subvolumes the btrfs filesystem is returned.
	 */
	Filesystem* get_filesystem();

	/**
	 * @copydoc get_filesystem()
	 */
	const Filesystem* get_filesystem() const;

	MountByType get_mount_by() const ST_DEPRECATED;
	void set_mount_by(MountByType mount_by) ST_DEPRECATED;

	const std::vector<std::string>& get_mount_opts() const ST_DEPRECATED;
	void set_mount_opts(const std::vector<std::string>& mount_opts) ST_DEPRECATED;

	const std::vector<std::string>& get_fstab_options() const ST_DEPRECATED;
	void set_fstab_options(const std::vector<std::string>& mount_opts) ST_DEPRECATED;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Mountable(Impl* impl);
	ST_NO_SWIG Mountable(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Mountable.
     *
     * @throw NullPointerException
     */
    bool is_mountable(const Device* device);

    /**
     * Converts pointer to Device to pointer to Mountable.
     *
     * @return Pointer to Mountable.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Mountable* to_mountable(Device* device);

    /**
     * @copydoc to_mountable(Device*)
     */
    const Mountable* to_mountable(const Device* device);

}

#endif
