/*
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


#ifndef STORAGE_UDF_H
#define STORAGE_UDF_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class for UDF filesystem. The library cannot create an UDF filesystem
     * on disk.
     *
     * @see https://en.wikipedia.org/wiki/Universal_Disk_Format
     */
    class Udf : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Udf. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Udf* create(Devicegraph* devicegraph);

	static Udf* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Udf* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Udf(Impl* impl);
	ST_NO_SWIG Udf(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Udf.
     *
     * @throw NullPointerException
     */
    bool is_udf(const Device* device);

    /**
     * Converts pointer to Device to pointer to Udf.
     *
     * @return Pointer to Udf.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Udf* to_udf(Device* device);

    /**
     * @copydoc to_udf(Device*)
     */
    const Udf* to_udf(const Device* device);

}

#endif
