/*
 * Copyright (c) 2015 Novell, Inc.
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

    class Xfs : public BlkFilesystem
    {
    public:

	static Xfs* create(Devicegraph* devicegraph);
	static Xfs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Xfs* clone() const override;

    protected:

	Xfs(Impl* impl);

    };


    bool is_xfs(const Device* device);

    Xfs* to_xfs(Device* device);
    const Xfs* to_xfs(const Device* device);

}

#endif
