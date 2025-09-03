/*
 * Copyright (c) 2024 SUSE LLC
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


#ifndef STORAGE_BCACHEFS_H
#define STORAGE_BCACHEFS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent a Bcachefs filesystem
     * https://en.wikipedia.org/wiki/Bcachefs in the devicegraph.
     */
    class Bcachefs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Bcachefs. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Bcachefs* create(Devicegraph* devicegraph);

	static Bcachefs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Bcachefs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Bcachefs(Impl* impl);

	ST_NO_SWIG Bcachefs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Bcachefs.
     *
     * @throw NullPointerException
     */
    bool is_bcachefs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Bcachefs.
     *
     * @return Pointer to Bcachefs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Bcachefs* to_bcachefs(Device* device);

    /**
     * @copydoc to_bcachefs(Device*)
     */
    const Bcachefs* to_bcachefs(const Device* device);

}

#endif
