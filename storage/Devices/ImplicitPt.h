/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#ifndef STORAGE_IMPLICIT_PT_H
#define STORAGE_IMPLICIT_PT_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    /**
     * Class to represent implicit partition table. An implicit partition
     * table does not exist on-disk so no operations are possible. It is
     * present on some DASDs where the kernel creates a partition all by
     * itself.
     *
     * See also doc/dasd.md.
     */
    class ImplicitPt : public PartitionTable
    {
    public:

	static ImplicitPt* create(Devicegraph* devicegraph);
	static ImplicitPt* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual ImplicitPt* clone() const override;

    protected:

	ImplicitPt(Impl* impl);

    };


    bool is_implicit_pt(const Device* device);

    /**
     * Converts pointer to Device to pointer to ImplicitPt.
     *
     * @return Pointer to ImplicitPt.
     * @throw DeviceHasWrongType, NullPointerException
     */
    ImplicitPt* to_implicit_pt(Device* device);

    /**
     * @copydoc to_implicit_pt(Device*)
     */
    const ImplicitPt* to_implicit_pt(const Device* device);

}

#endif
