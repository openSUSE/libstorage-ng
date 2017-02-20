/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_SWAP_H
#define STORAGE_SWAP_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    class Swap : public BlkFilesystem
    {
    public:

	static Swap* create(Devicegraph* devicegraph);
	static Swap* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 15; }

	bool supports_uuid() const override { return true; }

	static std::vector<Swap*> get_all(Devicegraph* devicegraph);
	static std::vector<const Swap*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Swap* clone() const override;

    protected:

	Swap(Impl* impl);

    };


    bool is_swap(const Device* device);

    Swap* to_swap(Device* device);
    const Swap* to_swap(const Device* device);

}

#endif
