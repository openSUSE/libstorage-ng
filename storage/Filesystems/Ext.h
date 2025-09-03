/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_EXT_H
#define STORAGE_EXT_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Abstract base class for Ext2, Ext3 and Ext4.
     */
    class Ext : public BlkFilesystem
    {

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Ext(Impl* impl);
	ST_NO_SWIG Ext(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Ext.
     *
     * @throw NullPointerException
     */
    bool is_ext(const Device* device);

    /**
     * Converts pointer to Device to pointer to Ext.
     *
     * @return Pointer to Ext.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Ext* to_ext(Device* device);

    /**
     * @copydoc to_ext(Device*)
     */
    const Ext* to_ext(const Device* device);

}

#endif
