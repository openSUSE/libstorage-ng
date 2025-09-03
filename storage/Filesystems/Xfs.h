/*
 * Copyright (c) 2015 Novell, Inc.
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


#ifndef STORAGE_XFS_H
#define STORAGE_XFS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent an XFS filesystem
     * https://en.wikipedia.org/wiki/Xfs in the devicegraph.
     */
    class Xfs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Xfs. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Xfs* create(Devicegraph* devicegraph);

	static Xfs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Xfs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Xfs(Impl* impl);
	ST_NO_SWIG Xfs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Xfs.
     *
     * @throw NullPointerException
     */
    bool is_xfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Xfs.
     *
     * @return Pointer to Xfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Xfs* to_xfs(Device* device);

    /**
     * @copydoc to_xfs(Device*)
     */
    const Xfs* to_xfs(const Device* device);

}

#endif
