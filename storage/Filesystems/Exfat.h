/*
 * Copyright (c) 2018 SUSE LLC
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


#ifndef STORAGE_EXFAT_H
#define STORAGE_EXFAT_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class to represent an exFAT filesystem
     * https://en.wikipedia.org/wiki/ExFAT in the devicegraph.
     */
    class Exfat : public BlkFilesystem
    {
    public:

	static Exfat* create(Devicegraph* devicegraph);
	static Exfat* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Exfat* clone() const override;

    protected:

	Exfat(Impl* impl);

    };


    bool is_exfat(const Device* device);

    /**
     * Converts pointer to Device to pointer to Exfat.
     *
     * @return Pointer to Exfat.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Exfat* to_exfat(Device* device);

    /**
     * @copydoc to_exfat(Device*)
     */
    const Exfat* to_exfat(const Device* device);

}

#endif
