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


#ifndef STORAGE_EXT2_H
#define STORAGE_EXT2_H


#include "storage/Filesystems/Ext.h"


namespace storage
{

    class Ext2 : public Ext
    {
    public:

	static Ext2* create(Devicegraph* devicegraph);
	static Ext2* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Ext2* clone() const override;

    protected:

	Ext2(Impl* impl);

    };


    bool is_ext2(const Device* device);

    Ext2* to_ext2(Device* device);
    const Ext2* to_ext2(const Device* device);

}

#endif
