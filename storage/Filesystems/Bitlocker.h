/*
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_BITLOCKER_H
#define STORAGE_BITLOCKER_H


#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    /**
     * Class for BitLocker filesystem. The library can only probe a
     * BitLocker filesystem on disk. It cannot mount, create or resize
     * it.
     */
    class Bitlocker : public BlkFilesystem
    {
    public:

	static Bitlocker* create(Devicegraph* devicegraph);
	static Bitlocker* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Bitlocker* clone() const override;

    protected:

	Bitlocker(Impl* impl);

    };


    /**
     * Checks whether device points to an Bitlocker.
     *
     * @throw NullPointerException
     */
    bool is_bitlocker(const Device* device);

    /**
     * Converts pointer to Device to pointer to Bitlocker.
     *
     * @return Pointer to Bitlocker.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Bitlocker* to_bitlocker(Device* device);

    /**
     * @copydoc to_bitlocker(Device*)
     */
    const Bitlocker* to_bitlocker(const Device* device);

}

#endif
