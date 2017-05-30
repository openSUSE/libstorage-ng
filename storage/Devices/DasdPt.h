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


#ifndef STORAGE_DASD_PT_H
#define STORAGE_DASD_PT_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    class DasdPt : public PartitionTable
    {
    public:

	static DasdPt* create(Devicegraph* devicegraph);
	static DasdPt* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual DasdPt* clone() const override;

    protected:

	DasdPt(Impl* impl);

    };


    bool is_dasd_pt(const Device* device);

    DasdPt* to_dasd_pt(Device* device);
    const DasdPt* to_dasd_pt(const Device* device);

}

#endif
