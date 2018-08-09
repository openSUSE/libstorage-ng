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


#ifndef STORAGE_EXT4_H
#define STORAGE_EXT4_H


#include "storage/Filesystems/Ext.h"


namespace storage
{

    /**
     * Class to represent an Ext4 filesystem
     * https://en.wikipedia.org/wiki/Ext4 in the devicegraph.
     */
    class Ext4 : public Ext
    {
    public:

	static Ext4* create(Devicegraph* devicegraph);
	static Ext4* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Ext4* clone() const override;

    protected:

	Ext4(Impl* impl);

    };


    bool is_ext4(const Device* device);

    /**
     * Converts pointer to Device to pointer to Ext4.
     *
     * @return Pointer to Ext4.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Ext4* to_ext4(Device* device);

    /**
     * @copydoc to_ext4(Device*)
     */
    const Ext4* to_ext4(const Device* device);

}

#endif
