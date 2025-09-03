/*
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_TMPFS_H
#define STORAGE_TMPFS_H


#include "storage/Filesystems/Filesystem.h"


namespace storage
{

    /**
     * Class to represent a tmpfs.
     *
     * The Tmpfs object should always have MountPoint as child. So when deleting the
     * MountPoint also delete the Tmpfs.
     *
     * Also see doc/tmpfs.md.
     */
    class Tmpfs : public Filesystem
    {
    public:

	/**
	 * Create a device of type Tmpfs.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Tmpfs* create(Devicegraph* devicegraph);

	static Tmpfs* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Tmpfses.
	 */
	static std::vector<Tmpfs*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Tmpfs*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Tmpfs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Tmpfs(Impl* impl);
	ST_NO_SWIG Tmpfs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Tmpfs.
     *
     * @throw NullPointerException
     */
    bool is_tmpfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Tmpfs.
     *
     * @return Pointer to Tmpfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Tmpfs* to_tmpfs(Device* device);

    /**
     * @copydoc to_tmpfs(Device*)
     */
    const Tmpfs* to_tmpfs(const Device* device);

}

#endif
