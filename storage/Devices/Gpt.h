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


#ifndef STORAGE_GPT_H
#define STORAGE_GPT_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    class Gpt : public PartitionTable
    {
    public:

	static Gpt* create(Devicegraph* devicegraph);
	static Gpt* load(Devicegraph* devicegraph, const xmlNode* node);

	bool is_enlarge() const;
	void set_enlarge(bool enlarge);

	/**
	 * Query the protective MBR boot flag.
	 */
	bool is_pmbr_boot() const;

	/**
	 * Set the protective MBR boot flag.
	 */
	void set_pmbr_boot(bool pmbr_boot);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Gpt* clone() const override;

    protected:

	Gpt(Impl* impl);

    };


    bool is_gpt(const Device* device);

    Gpt* to_gpt(Device* device);
    const Gpt* to_gpt(const Device* device);

}

#endif
