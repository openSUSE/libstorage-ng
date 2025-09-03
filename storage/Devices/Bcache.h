/*
 * Copyright (c) [2016-2023] SUSE LLC
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
#include "storage/Utils/Swig.h"


namespace storage
{

    class BcacheCset;


    /**
     * Bcache types
     */
    enum class BcacheType {
	BACKED, FLASH_ONLY
    };


    /**
     * Convert the BcacheType bcache_type to a string.
     *
     * @see BcacheType
     */
    std::string get_bcache_type_name(BcacheType bcache_type);


    /**
     * The Cache mode attribute.
     */
    enum class CacheMode {
	WRITETHROUGH, WRITEBACK, WRITEAROUND, NONE
    };


    /**
     * Convert the CacheMode cache_mode to a string.
     *
     * @see CacheMode
     */
    std::string get_cache_mode_name(CacheMode cache_mode);


    /**
     * A <a href="https://www.kernel.org/doc/Documentation/bcache.txt">bcache</a> device.
     *
     * Bcache technology supports two kinds of bcache devices. Bcache devices with a backing
     * device and also bcache devices directly created over a caching set (without backing device
     * associated to it). This second type is known as Flash-only bcache.
     */
    class Bcache : public Partitionable
    {
    public:

	/**
	 * Create a device of type Bcache. Usually this function is not called
	 * directly. Instead BlkDevice::create_bcache() is called.
	 */
	static Bcache* create(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static Bcache* create(Devicegraph* devicegraph, const std::string& name, BcacheType type);

	static Bcache* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the bcache type.
	 *
	 * @see BcacheType
	 */
	BcacheType get_type() const;

	/**
	 * Get the number of the bcache.
	 */
	unsigned int get_number() const;

	/**
	 * Get the BlkDevice used as backing device.
	 *
	 * This method does not make sense for Flash-only bcache devices.
	 *
	 * @throw DeviceNotFound
	 */
	const BlkDevice* get_backing_device() const;

	/**
	 * @copydoc get_backing_device
	 */
	const BlkDevice* get_blk_device() const ST_DEPRECATED;

	/**
	 * Returns true if a caching set is attached.
	 *
	 * Note that a Flash-only bcache is created over a caching set, so this method should
	 * always return true for Flash-only bcache devices.
	 */
	bool has_bcache_cset() const;

	/**
	 * Get the caching set associated with this bcache device.
	 *
	 * Note that a bcache device can be created without a caching set associated to it.
	 * In case of a Flash-only bcache, there is always a caching set holding it.
	 */
	const BcacheCset* get_bcache_cset() const;

	/**
	 * Associates a caching set to the bcache.
	 *
	 * This method does not make sense for Flash-only bcache devices.
	 *
	 * @throw LogicException, Exception
	 */
	void attach_bcache_cset(BcacheCset* bcache_cset);

	/**
	 * @copydoc attach_bcache_cset
	 */
	void add_bcache_cset(BcacheCset* bcache_cset) ST_DEPRECATED;

	/**
	 * Removes association with a caching set.
	 *
	 * This method does not make sense for Flash-only bcache devices.
	 *
	 * @throw LogicException, Exception
	 */
	void detach_bcache_cset();

	/**
	 * @copydoc detach_bcache_cset
	 */
	void remove_bcache_cset() ST_DEPRECATED;

	/**
	 * Returns cache mode attribute.
	 *
	 * This method does not make sense for Flash-only bcache devices.
	 */
	CacheMode get_cache_mode() const;

	/**
	 * Sets cache mode attribute.
	 *
	 * This method does not make sense for Flash-only bcache devices.
	 *
	 * @param[in] mode target cache mode
	 */
	void set_cache_mode(CacheMode mode);

	/**
	 * Returns size of sequential_cutoff attribute.
	 *
	 * This method does not make sense for Flash-only bcache devices.
	 */
	unsigned long long get_sequential_cutoff() const;

	/**
	 * Returns percent of writeback dirty pages.
	 *
	 * This method does not make sense for Flash-only bcache devices.
	 */
	unsigned get_writeback_percent() const;

	/**
	 * Get all bcaches.
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
	 * Find a bcache by its name.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Bcache* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const Bcache* find_by_name(const Devicegraph* devicegraph, const std::string& name);

	/**
	 * Find a free name for a bcache, e.g. "/dev/bcache2". A free name might not be
	 * available.
	 *
	 * @throw Exception
	 */
	static std::string find_free_name(const Devicegraph* devicegraph);

	/**
	 * Compare (less than) two bcaches by number.
	 *
	 * @see get_number()
	 */
	static bool compare_by_number(const Bcache* lhs, const Bcache* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Bcache* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Bcache(Impl* impl);
	ST_NO_SWIG Bcache(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Bcache.
     *
     * @throw NullPointerException
     */
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
