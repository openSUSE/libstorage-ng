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


#ifndef STORAGE_BCACHE_CSET_H
#define STORAGE_BCACHE_CSET_H


#include "storage/Devices/Device.h"


namespace storage
{


    //! A Bcache cache set.
    class BcacheCset : public Device
    {
    public:

	static BcacheCset* create(Devicegraph* devicegraph);
	static BcacheCset* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual void check() const override;

	const std::string& get_uuid() const;
	void set_uuid(const std::string& uuid);

	/**
	 * Sorted by uuid.
	 */
	static std::vector<BcacheCset*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const BcacheCset*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual BcacheCset* clone() const override;

    protected:

	BcacheCset(Impl* impl);

    };


    bool is_bcache_cset(const Device* device);

    BcacheCset* to_bcache_cset(Device* device);
    const BcacheCset* to_bcache_cset(const Device* device);

}

#endif
