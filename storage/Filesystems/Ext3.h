/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_EXT3_H
#define STORAGE_EXT3_H


#include "storage/Filesystems/Ext.h"


namespace storage
{

    /**
     * Class to represent an Ext3 filesystem
     * https://en.wikipedia.org/wiki/Ext3 in the devicegraph.
     */
    class Ext3 : public Ext
    {
    public:

	/**
	 * Create a device of type Ext3. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Ext3* create(Devicegraph* devicegraph);

	static Ext3* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Ext3* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Ext3(Impl* impl);
	ST_NO_SWIG Ext3(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Ext3.
     *
     * @throw NullPointerException
     */
    bool is_ext3(const Device* device);

    /**
     * Converts pointer to Device to pointer to Ext3.
     *
     * @return Pointer to Ext3.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Ext3* to_ext3(Device* device);

    /**
     * @copydoc to_ext3(Device*)
     */
    const Ext3* to_ext3(const Device* device);

}

#endif
