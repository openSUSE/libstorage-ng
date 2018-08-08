/*
 * Copyright (c) 2018 SUSE LLC
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


#ifndef STORAGE_JFS_H
#define STORAGE_JFS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent a JFS filesystem
     * https://en.wikipedia.org/wiki/JFS_(file_system) in the
     * devicegraph.
     */
    class Jfs : public BlkFilesystem
    {
    public:

	static Jfs* create(Devicegraph* devicegraph);
	static Jfs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Jfs* clone() const override;

    protected:

	Jfs(Impl* impl);

    };


    bool is_jfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Jfs.
     *
     * @return Pointer to Jfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Jfs* to_jfs(Device* device);

    /**
     * @copydoc to_jfs(Device*)
     */
    const Jfs* to_jfs(const Device* device);

}

#endif
