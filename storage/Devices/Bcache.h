/*
 * Copyright (c) [2016,2018] SUSE LLC
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


#ifndef STORAGE_BCACHE_H
#define STORAGE_BCACHE_H


#include "storage/Devices/Partitionable.h"


namespace storage
{

    class BcacheCset;


    /**
     * A <a href="https://www.kernel.org/doc/Documentation/bcache.txt">bcache</a>
     * device.
     */
    class Bcache : public Partitionable
    {
    public:

	static Bcache* create(Devicegraph* devicegraph, const std::string& dm_name);
	static Bcache* load(Devicegraph* devicegraph, const xmlNode* node);

	unsigned int get_number() const;

	/**
	 * Get the BlkDevice used as backing device.
	 */
	const BlkDevice* get_blk_device() const;

	/**
	 * Returns true if a BcacheCset is attached.
	 */
	bool has_bcache_cset() const;

	/**
	 * Get the BcacheCset used as cache.
	 */
	const BcacheCset* get_bcache_cset() const;

	/**
	 * Attach a BcacheCset to the Bcache.
	 *
	 * @throw Exception
	 */
	void attach_bcache_cset(BcacheCset* bcache_cset);

	/**
	 * Get all Bcaches.
	 */
	static std::vector<Bcache*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Bcache*> get_all(const Devicegraph* devicegraph);

	/**
	 * Find a Bcache by its name.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Bcache* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const Bcache* find_by_name(const Devicegraph* devicegraph, const std::string& name);

	/**
	 * Compare (less than) two Bcaches by number.
	 */
	static bool compare_by_number(const Bcache* lhs, const Bcache* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Bcache* clone() const override;

    protected:

	Bcache(Impl* impl);

    };


    bool is_bcache(const Device* device);

    /**
     * Converts pointer to Device to pointer to Bcache.
     *
     * @return Pointer to Bcache.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Bcache* to_bcache(Device* device);

    /**
     * @copydoc to_bcache(Device*)
     */
    const Bcache* to_bcache(const Device* device);

}

#endif
