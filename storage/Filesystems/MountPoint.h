/*
 * Copyright (c) [2017-2021] SUSE LLC
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


#ifndef STORAGE_MOUNT_POINT_H
#define STORAGE_MOUNT_POINT_H


#include "storage/Devices/Device.h"
#include "storage/Filesystems/Mountable.h"


namespace storage
{

    class Devicegraph;


    class InvalidMountPointPath : public Exception
    {
    public:

        InvalidMountPointPath(const std::string& path);
    };


    /**
     * Class to represent a mount point.
     */
    class MountPoint : public Device
    {
    public:

	/**
	 * @throw InvalidMountPointPath
	 */
	static MountPoint* create(Devicegraph* devicegraph, const std::string& path);

	static MountPoint* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all MountPoints.
	 */
	static std::vector<MountPoint*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const MountPoint*> get_all(const Devicegraph* devicegraph);

	const std::string& get_path() const;

	/**
	 * @throw InvalidMountPointPath
	 */
	void set_path(const std::string& path);

	/**
	 * Return whether the path of the mount point will be prefixed with the rootprefix
	 * during mount operations.
	 *
	 * The default value is true (for historic reasons).
	 *
	 * @see Environment::get_rootprefix()
	 */
	bool is_rootprefixed() const;

	/**
	 * Sets whether the path is rootprefixed.
	 *
	 * @see is_rootprefixed()
	 */
	void set_rootprefixed(bool rootprefixed);

	/**
	 * Return normalized form of path.
	 */
	static std::string normalize_path(const std::string& path);

	/**
	 * Get the mount-by method.
	 */
	MountByType get_mount_by() const;

	/**
	 * Set the mount-by method.
	 */
	void set_mount_by(MountByType mount_by);

	/**
	 * Set the mount-by method to the global default, see
	 * Storage::get_default_mount_by().
	 */
	void set_default_mount_by();

	/**
	 * Returns the possible mount-by methods for the mount point.
	 *
	 * LABEL is included even if the filesystem label is not set.
	 *
	 * @throw Exception
	 */
	std::vector<MountByType> possible_mount_bys() const;

	/**
	 * Get the mount options.
	 */
	const std::vector<std::string>& get_mount_options() const;

	/**
	 * Set the mount options. Avoid overriding the subvol option for btrfs
	 * subvolumes unless you are certain what you are doing.
	 */
	void set_mount_options(const std::vector<std::string>& mount_options);

	/**
	 * Default mount options. So far the default mount options only contain
	 * the subvol for btrfs subvolumes.
	 */
	std::vector<std::string> default_mount_options() const;

	/**
	 * Set the mount options to the default mount options.
	 *
	 * @see default_mount_options()
	 */
	void set_default_mount_options();

	/**
	 * Get the filesystem type used to mount the device, as specified in
	 * fstab(5) and/or in the mount(8) command.
	 */
	FsType get_mount_type() const;

	/**
	 * Set the filesystem type to be used to mount the device. Setting the
	 * filesystem type to FsType::UNKNOWN is not allowed.
	 *
	 * @throw Exception
	 */
	void set_mount_type(FsType mount_type);

	/**
	 * Get the freq field from fstab(5). The freq field is used by the
	 * dump(8) command to determine which filesystems need to be
	 * dumped. The field is likely obsolete.
	 */
	int get_freq() const;

	/**
	 * Set the freq field.
	 *
	 * @see get_freq()
	 */
	void set_freq(int freq);

	/**
	 * Get the passno field from fstab(5). The passno field is used by the
	 * fsck(8) program to determine the order in which filesystem checks
	 * are done at reboot time.
	 */
	int get_passno() const;

	/**
	 *
	 * @see get_passno()
	 */
	void set_passno(int passno);

	/**
	 * Return whether the mount point is active (mounted).
	 */
	bool is_active() const;

	/**
	 * Sets whether the mount point is active (mounted).
	 */
	void set_active(bool active);

	/**
	 * Query whether the mount point is present (probed devicegraph) or
	 * will be present (staging devicegraph) in /etc/fstab.
	 */
	bool is_in_etc_fstab() const;

	/**
	 * Set whether the mount point will be present in /etc/fstab.
	 */
	void set_in_etc_fstab(bool in_etc_fstab);

	/**
	 * Checks whether the mount point has a mountable. This is usually true
	 * unless the devicegraph is broken.
	 */
	bool has_mountable() const;

	/**
	 * Return the mountable of the mount point.
	 *
	 * @throw Exception
	 */
	Mountable* get_mountable();

	/**
	 * @copydoc get_mountable()
	 */
	const Mountable* get_mountable() const;

	/**
	 * Return the filesystem of mountable of the mount point.
	 *
	 * @throw Exception
	 */
	Filesystem* get_filesystem();

	/**
	 * @copydoc get_filesystem()
	 */
	const Filesystem* get_filesystem() const;

	/**
	 * Find all MountPoints with path.
	 */
	static std::vector<const MountPoint*> find_by_path(const Devicegraph* devicegraph,
							   const std::string& path);

	/**
	 * Immediately activate (mount) the mount point. In contrast to
	 * set_active() this function acts immediately and does not require
	 * calling commit().
	 *
	 * The mount point object must exist in the probed devicegraph.
	 *
	 * @throw Exception
	 */
	void immediate_activate();

	/**
	 * Immediately deactivate (unmount) the mount point object. In
	 * contrast to set_active() this function acts immediately and does
	 * not require calling commit().
	 *
	 * The mount point object must exist in the probed devicegraph.
	 *
	 * @throw Exception
	 */
	void immediate_deactivate();

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual MountPoint* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	MountPoint(Impl* impl);
	ST_NO_SWIG MountPoint(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a MountPoint.
     *
     * @throw NullPointerException
     */
    bool is_mount_point(const Device* device);

    /**
     * Converts pointer to Device to pointer to Mountpoint.
     *
     * @return Pointer to Mountpoint.
     * @throw DeviceHasWrongType, NullPointerException
     */
    MountPoint* to_mount_point(Device* device);

    /**
     * @copydoc to_mount_point(Device*)
     */
    const MountPoint* to_mount_point(const Device* device);

}

#endif
