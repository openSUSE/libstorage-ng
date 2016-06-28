/*
 * Copyright (c) 2016 SUSE LLC
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


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    //! A bcache device.
    class Bcache : public BlkDevice
    {
    public:

	static Bcache* create(Devicegraph* devicegraph, const std::string& dm_name);
	static Bcache* load(Devicegraph* devicegraph, const xmlNode* node);

	unsigned int get_number() const;

	/**
	 * Sorted by number.
	 */
	static std::vector<Bcache*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Bcache*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Bcache* clone() const override;

    protected:

	Bcache(Impl* impl);

    };


    bool is_bcache(const Device* device);

    Bcache* to_bcache(Device* device);
    const Bcache* to_bcache(const Device* device);

}

#endif
