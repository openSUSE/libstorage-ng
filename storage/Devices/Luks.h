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


#ifndef STORAGE_LUKS_H
#define STORAGE_LUKS_H


#include "storage/Devices/Encryption.h"


namespace storage
{

    /**
     * An luks encryption layer on a block device
     *
     * Do not resize the Luks object but the underlying BlkDevice object.
     */
    class Luks : public Encryption
    {
    public:

	static Luks* create(Devicegraph* devicegraph, const std::string& dm_name);
	static Luks* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Sorted by dm-table-name.
	 */
	static std::vector<Luks*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Luks*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Luks* clone() const override;

    protected:

	Luks(Impl* impl);

    };


    bool is_luks(const Device* device);

    Luks* to_luks(Device* device);
    const Luks* to_luks(const Device* device);

}

#endif
