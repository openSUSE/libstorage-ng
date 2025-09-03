/*
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_BCACHE_CSET_H
#define STORAGE_BCACHE_CSET_H


#include "storage/Devices/Device.h"


namespace storage
{

    class BlkDevice;
    class Bcache;


    /**
     * A <a href="https://www.kernel.org/doc/Documentation/bcache.txt">bcache</a>
     * cache set.
     */
    class BcacheCset : public Device
    {
    public:

	/**
	 * Create a device of type BcacheCset. Usually this function is not called
	 * directly. Instead BlkDevice::create_bcache_cset() is called.
	 */
	static BcacheCset* create(Devicegraph* devicegraph);

	static BcacheCset* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Return the UUID of the BcacheCset.
	 */
	const std::string& get_uuid() const;

	/**
	 * Set the UUID of the BcacheCset.
	 *
	 * The UUID on disk cannot be change. This function is only for testsuites.
	 */
	void set_uuid(const std::string& uuid);

	/**
	 * Returns the BlkDevices used by this BcacheCset.
	 */
	std::vector<const BlkDevice*> get_blk_devices() const;

	/**
	 * Returns the Bcaches using this BcacheCset.
	 */
	std::vector<const Bcache*> get_bcaches() const;

	/**
	 * Get all BcacheCsets.
	 */
	static std::vector<BcacheCset*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const BcacheCset*> get_all(const Devicegraph* devicegraph);

	/**
	 * Find a BcacheCset by its UUID.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static BcacheCset* find_by_uuid(Devicegraph* devicegraph, const std::string& uuid);

	/**
	 * @copydoc find_by_uuid
	 */
	static const BcacheCset* find_by_uuid(const Devicegraph* devicegraph, const std::string& uuid);

	/**
	 * Compare (less than) two BcacheCsets by UUID.
	 *
	 * @see get_uuid()
	 */
	static bool compare_by_uuid(const BcacheCset* lhs, const BcacheCset* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual BcacheCset* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	BcacheCset(Impl* impl);
	ST_NO_SWIG BcacheCset(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a BcacheCset.
     *
     * @throw NullPointerException
     */
    bool is_bcache_cset(const Device* device);

    /**
     * Converts pointer to Device to pointer to BcacheCset.
     *
     * @return Pointer to BcacheCset.
     * @throw DeviceHasWrongType, NullPointerException
     */
    BcacheCset* to_bcache_cset(Device* device);

    /**
     * @copydoc to_bcache_cset(Device*)
     */
    const BcacheCset* to_bcache_cset(const Device* device);

}

#endif
