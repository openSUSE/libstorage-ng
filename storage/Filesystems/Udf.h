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


#ifndef STORAGE_UDF_H
#define STORAGE_UDF_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class for UDF filesystem. The library cannot create an UDF filesystem
     * on disk.
     */
    class Udf : public BlkFilesystem
    {
    public:

	static Udf* create(Devicegraph* devicegraph);
	static Udf* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Udf* clone() const override;

    protected:

	Udf(Impl* impl);

    };


    bool is_udf(const Device* device);

    Udf* to_udf(Device* device);
    const Udf* to_udf(const Device* device);

}

#endif
