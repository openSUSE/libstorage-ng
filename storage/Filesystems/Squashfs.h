/*
 * Copyright (c) [2017-2026] SUSE LLC
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


#ifndef STORAGE_SQUASHFS_H
#define STORAGE_SQUASHFS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class for Squashfs filesystem. The library cannot create a Squashfs
     * filesystem on disk.
     *
     * @see https://en.wikipedia.org/wiki/SquashFS
     */
    class Squashfs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Squashfs. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Squashfs* create(Devicegraph* devicegraph);

	static Squashfs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Squashfs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Squashfs(Impl* impl);
	ST_NO_SWIG Squashfs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Squashfs.
     *
     * @throw NullPointerException
     */
    bool is_squashfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Squashfs.
     *
     * @return Pointer to Squashfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Squashfs* to_squashfs(Device* device);

    /**
     * @copydoc to_squashfs(Device*)
     */
    const Squashfs* to_squashfs(const Device* device);

}

#endif
