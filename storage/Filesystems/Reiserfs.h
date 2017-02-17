/*
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


#ifndef STORAGE_REISERFS_H
#define STORAGE_REISERFS_H


#include "storage/Filesystems/Filesystem.h"


namespace storage
{

    class Reiserfs : public Filesystem
    {
    public:

	static Reiserfs* create(Devicegraph* devicegraph);
	static Reiserfs* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 16; }

	bool supports_uuid() const override { return true; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Reiserfs* clone() const override;

    protected:

	Reiserfs(Impl* impl);

    };


    bool is_reiserfs(const Device* device);

    Reiserfs* to_reiserfs(Device* device);
    const Reiserfs* to_reiserfs(const Device* device);

}

#endif
