/*
 * Copyright (c) [2019] SUSE LLC
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


#ifndef STORAGE_BACKED_BCACHE_H
#define STORAGE_BACKED_BCACHE_H


#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/XmlFile.h"

namespace storage
{

    /**
     * The Cache mode attribute.
     */
    enum class CacheMode {
	WRITETHROUGH, WRITEBACK, WRITEAROUND, NONE
    };


    /**
     * A <a href="https://www.kernel.org/doc/Documentation/bcache.txt">Bcache</a>
     * device with a backing device (Backed Bcache).
     *
     * Bcache technology also supports another kind of bcache devices directly created over
     * a caching set (without backing device associated to it), see FlashBcache.
     */
    class BackedBcache : public Bcache
    {
    public:

	static BackedBcache* create(Devicegraph* devicegraph, const std::string& name);
	static BackedBcache* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the BlkDevice used as backing device.
	 */
	const BlkDevice* get_backing_device() const;

	/**
	 * Returns true if a BcacheCset is attached.
	 */
	bool has_bcache_cset() const;

	/**
	 * Attach a BcacheCset to the Bcache.
	 *
	 * @throw Exception
	 */
	void attach_bcache_cset(BcacheCset* bcache_cset);

	/**
	 * Returns cache mode attribute.
	 */
	CacheMode get_cache_mode() const;

	/**
	 * Sets cache mode attribute
	 *
	 * @param[in] mode target cache mode
	 */
	void set_cache_mode(CacheMode mode);

	/**
	 * Returns size of sequential_cutoff attribute.
	 */
	unsigned long long get_sequential_cutoff() const;

	/**
	 * Sets sequential_cutoff attribute
	 *
	 * @param[in] size size in a bytes
	 */
	void set_sequential_cutoff(unsigned long long size);

	/**
	 * Returns percent of writeback dirty pages.
	 */
	unsigned get_writeback_percent() const;

	/**
	 * Sets writeback percent attribute
	 *
	 * @param[in] percent target cache mode
	 */
	void set_writeback_percent(unsigned percent);

	/**
	 * Get all Backed Bcaches.
	 */
	static std::vector<BackedBcache*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const BackedBcache*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual BackedBcache* clone() const override;

    protected:

	BackedBcache(Impl* impl);

    };


    bool is_backed_bcache(const Device* device);

    /**
     * Converts pointer to Device to pointer to BackedBcache.
     *
     * @return Pointer to BackedBcache.
     * @throw DeviceHasWrongType, NullPointerException
     */
    BackedBcache* to_backed_bcache(Device* device);

    /**
     * @copydoc to_backed_bcache(Device*)
     */
    const BackedBcache* to_backed_bcache(const Device* device);

}

#endif
