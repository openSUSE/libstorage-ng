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


#ifndef STORAGE_VFAT_H
#define STORAGE_VFAT_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent an VFAT filesystem
     * https://en.wikipedia.org/wiki/Vfat in the devicegraph.
     */
    class Vfat : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Vfat. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Vfat* create(Devicegraph* devicegraph);

	static Vfat* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Vfat* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Vfat(Impl* impl);
	ST_NO_SWIG Vfat(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Vfat.
     *
     * @throw NullPointerException
     */
    bool is_vfat(const Device* device);

    /**
     * Converts pointer to Device to pointer to Vfat.
     *
     * @return Pointer to Vfat.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Vfat* to_vfat(Device* device);

    /**
     * @copydoc to_vfat(Device*)
     */
    const Vfat* to_vfat(const Device* device);

}

#endif
