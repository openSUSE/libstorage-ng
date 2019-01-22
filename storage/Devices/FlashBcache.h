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


#ifndef STORAGE_FLASH_BCACHE_H
#define STORAGE_FLASH_BCACHE_H


#include "storage/Devices/Bcache.h"
#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{
    /**
     * A <a href="https://evilpiepirate.org/git/linux-bcache.git/tree/drivers/md/bcache/bcache.h#n31">
     * Flash-only Bcache</a> device.
     *
     * A Flash-only Bcache is directly created over a caching set. There is no backing device associated
     * to it. A caching set can be used for caching and, at the same time, it can hold several Flash-only
     * Bcache devices.
     */
    class FlashBcache : public Bcache
    {
    public:

	static FlashBcache* create(Devicegraph* devicegraph, const std::string& name);
	static FlashBcache* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Flash-only Bcaches.
	 */
	static std::vector<FlashBcache*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const FlashBcache*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual FlashBcache* clone() const override;

    protected:

	FlashBcache(Impl* impl);
    };


    bool is_flash_bcache(const Device* device);

    /**
     * Converts pointer to Device to pointer to FlashBcache.
     *
     * @return Pointer to FlashBcache.
     * @throw DeviceHasWrongType, NullPointerException
     */
    FlashBcache* to_flash_bcache(Device* device);

    /**
     * @copydoc to_bcache(Device*)
     */
    const FlashBcache* to_flash_bcache(const Device* device);

}

#endif
