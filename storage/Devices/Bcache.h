/*
 * Copyright (c) [2016-2019] SUSE LLC
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
#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"


namespace storage
{

    class BcacheCset;


    //! Bcache Type.
    enum class BcacheType {
	BACKED, FLASHONLY
    };

    std::string get_bcache_type_name(BcacheType type);


    /**
     * Abstract class. See BackedBcache and FlashBcache.
     */
    class Bcache : public Partitionable
    {
    public:

	BcacheType get_type() const;

	unsigned int get_number() const;

	/**
	 * Get the BcacheCset associated with this Bcache device.
	 * Note that for a BackedBcache, the BcacheCset is used for caching, but for a
	 * FlashBcache it represents the cset in which the bcache is created.
	 */
	const BcacheCset* get_bcache_cset() const;

	/**
	 * Get all Bcaches.
	 */
	static std::vector<Bcache*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Bcache*> get_all(const Devicegraph* devicegraph);

	/**
	 * Fix the numeric ids of bcache devices so that there are no holes in the id sequence
	 * for in-memory bcache devices. Called automatically when a device is deleted and
	 * a hole can appear.
	 *
	 * Following rules apply to bcaches:
	 * - An existing bcache keeps its number until reboot
	 * - A newly created device always uses the lowest available number
	 *   So the devices in memory always fill holes in the numbering sequence
	 */
	static void reassign_numbers(Devicegraph* devicegraph);

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
	 * Find a free name for a Bcache.
	 */
	static std::string find_free_name(const Devicegraph* devicegraph);

	/**
	 * Compare (less than) two Bcaches by number.
	 */
	static bool compare_by_number(const Bcache* lhs, const Bcache* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

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
