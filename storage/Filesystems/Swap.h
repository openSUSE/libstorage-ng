/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_SWAP_H
#define STORAGE_SWAP_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent a swap filesystem in the devicegraph.
     *
     * The mount point path of a Swap must be "swap" or "none".
     */
    class Swap : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Swap. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Swap* create(Devicegraph* devicegraph);

	static Swap* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Swaps.
	 */
	static std::vector<Swap*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Swap*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Swap* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Swap(Impl* impl);
	ST_NO_SWIG Swap(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Swap.
     *
     * @throw NullPointerException
     */
    bool is_swap(const Device* device);

    /**
     * Converts pointer to Device to pointer to Swap.
     *
     * @return Pointer to Swap.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Swap* to_swap(Device* device);

    /**
     * @copydoc to_swap(Device*)
     */
    const Swap* to_swap(const Device* device);

}

#endif
