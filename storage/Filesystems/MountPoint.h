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


#ifndef STORAGE_MOUNT_POINT_H
#define STORAGE_MOUNT_POINT_H


#include "storage/Devices/Device.h"
#include "storage/Filesystems/Mountable.h"


namespace storage
{

    class Devicegraph;


    /**
     * Class to represent a mount point.
     */
    class MountPoint : public Device
    {
    public:

	static MountPoint* create(Devicegraph* devicegraph, const std::string& path);
	static MountPoint* load(Devicegraph* devicegraph, const xmlNode* node);

	static std::vector<MountPoint*> get_all(Devicegraph* devicegraph);
	static std::vector<const MountPoint*> get_all(const Devicegraph* devicegraph);

	const std::string& get_path() const;
	void set_path(const std::string& path);

	MountByType get_mount_by() const;
	void set_mount_by(const MountByType mount_by);

	void set_default_mount_by();

	const std::vector<std::string>& get_mount_options() const;
	void set_mount_options(const std::vector<std::string>& mount_options);

	void set_default_mount_options();

	int get_freq() const;

	int get_passno() const;

	bool is_active() const;
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
	 * Checks whether the mount point has a mountable. This is usally true
	 * unless the devicegraph is broken.
	 */
	bool has_mountable() const;

	/**
	 * Return the mountable of the mount point.
	 */
	Mountable* get_mountable();

	/**
	 * @copydoc get_mountable()
	 */
	const Mountable* get_mountable() const;

	static std::vector<const MountPoint*> find_by_path(const Devicegraph* devicegraph,
							   const std::string& path);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual MountPoint* clone() const override;

    protected:

	MountPoint(Impl* impl);

    };


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
