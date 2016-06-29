/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_MSDOS_H
#define STORAGE_MSDOS_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    class Msdos : public PartitionTable
    {
    public:

	static Msdos* create(Devicegraph* devicegraph);
	static Msdos* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Msdos* clone() const override;

	unsigned long get_minimal_mbr_gap() const;
	void set_minimal_mbr_gap(unsigned long minimal_mbr_gap);

    protected:

	Msdos(Impl* impl);

    };


    bool is_msdos(const Device* device);

    Msdos* to_msdos(Device* device);
    const Msdos* to_msdos(const Device* device);

}


#endif
