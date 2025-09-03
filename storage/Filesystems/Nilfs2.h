/*
 * Copyright (c) 2022 SUSE LLC
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


#ifndef STORAGE_NILFS2_H
#define STORAGE_NILFS2_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent a NILFS2 (https://en.wikipedia.org/wiki/NILFS2)
     * in the devicegraph.
     */
    class Nilfs2 : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Nilfs2. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Nilfs2* create(Devicegraph* devicegraph);

	static Nilfs2* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Nilfs2* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Nilfs2(Impl* impl);
	ST_NO_SWIG Nilfs2(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Nilfs2.
     *
     * @throw NullPointerException
     */
    bool is_nilfs2(const Device* device);

    /**
     * Converts pointer to Device to pointer to Nilfs2.
     *
     * @return Pointer to Nilfs2.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Nilfs2* to_nilfs2(Device* device);

    /**
     * @copydoc to_nilfs2(Device*)
     */
    const Nilfs2* to_nilfs2(const Device* device);

}

#endif
