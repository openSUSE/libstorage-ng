/*
 * Copyright (c) [2017-2026] SUSE LLC
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


#ifndef STORAGE_EROFS_H
#define STORAGE_EROFS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class for EROFS filesystem. The library cannot create an EROFS
     * filesystem on disk.
     *
     * @see https://en.wikipedia.org/wiki/EROFS
     */
    class Erofs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type EROFS. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Erofs* create(Devicegraph* devicegraph);

	static Erofs* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Erofs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Erofs(Impl* impl);
	ST_NO_SWIG Erofs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Erofs.
     *
     * @throw NullPointerException
     */
    bool is_erofs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Erofs.
     *
     * @return Pointer to Erofs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Erofs* to_erofs(Device* device);

    /**
     * @copydoc to_erofs(Device*)
     */
    const Erofs* to_erofs(const Device* device);

}

#endif
