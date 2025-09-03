/*
 * Copyright (c) [2018-2020] SUSE LLC
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


#ifndef STORAGE_F2FS_H
#define STORAGE_F2FS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent a F2FS (https://en.wikipedia.org/wiki/F2FS)
     * in the devicegraph.
     */
    class F2fs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type F2fs. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static F2fs* create(Devicegraph* devicegraph);

	static F2fs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual F2fs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	F2fs(Impl* impl);
	ST_NO_SWIG F2fs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a F2fs.
     *
     * @throw NullPointerException
     */
    bool is_f2fs(const Device* device);

    /**
     * Converts pointer to Device to pointer to F2fs.
     *
     * @return Pointer to F2fs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    F2fs* to_f2fs(Device* device);

    /**
     * @copydoc to_f2fs(Device*)
     */
    const F2fs* to_f2fs(const Device* device);

}

#endif
