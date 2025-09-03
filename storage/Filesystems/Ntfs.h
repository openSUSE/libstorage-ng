/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_NTFS_H
#define STORAGE_NTFS_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent an NTFS filesystem
     * https://en.wikipedia.org/wiki/NTFS in the devicegraph.
     */
    class Ntfs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Ntfs. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Ntfs* create(Devicegraph* devicegraph);

	static Ntfs* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Ntfses.
	 */
	static std::vector<Ntfs*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Ntfs*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Ntfs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Ntfs(Impl* impl);
	ST_NO_SWIG Ntfs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Ntfs.
     *
     * @throw NullPointerException
     */
    bool is_ntfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Ntfs.
     *
     * @return Pointer to Ntfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Ntfs* to_ntfs(Device* device);

    /**
     * @copydoc to_ntfs(Device*)
     */
    const Ntfs* to_ntfs(const Device* device);

}

#endif
