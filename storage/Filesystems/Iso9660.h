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


#ifndef STORAGE_ISO9660_H
#define STORAGE_ISO9660_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class for ISO9660 filesystem. The library cannot create an ISO9660
     * filesystem on disk.
     *
     * @see https://en.wikipedia.org/wiki/ISO_9660
     */
    class Iso9660 : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Iso9660. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Iso9660* create(Devicegraph* devicegraph);

	static Iso9660* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Iso9660* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Iso9660(Impl* impl);
	ST_NO_SWIG Iso9660(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Iso9660.
     *
     * @throw NullPointerException
     */
    bool is_iso9660(const Device* device);

    /**
     * Converts pointer to Device to pointer to Iso9660.
     *
     * @return Pointer to Iso9660.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Iso9660* to_iso9660(Device* device);

    /**
     * @copydoc to_iso9660(Device*)
     */
    const Iso9660* to_iso9660(const Device* device);

}

#endif
