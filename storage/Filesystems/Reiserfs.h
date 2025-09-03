/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#ifndef STORAGE_REISERFS_H
#define STORAGE_REISERFS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent a ReiserFS filesystem
     * https://en.wikipedia.org/wiki/ReiserFS in the devicegraph.
     */
    class Reiserfs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Reiserfs. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Reiserfs* create(Devicegraph* devicegraph);

	static Reiserfs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Reiserfs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Reiserfs(Impl* impl);
	ST_NO_SWIG Reiserfs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Reiserfs.
     *
     * @throw NullPointerException
     */
    bool is_reiserfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Reiserfs.
     *
     * @return Pointer to Reiserfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Reiserfs* to_reiserfs(Device* device);

    /**
     * @copydoc to_reiserfs(Device*)
     */
    const Reiserfs* to_reiserfs(const Device* device);

}

#endif
