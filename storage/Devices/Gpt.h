/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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

	/**
	 * Create a device of type Gpt. Usually this function is not called
	 * directly. Instead Partitionable::create_partition_table() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Gpt* create(Devicegraph* devicegraph);

	static Gpt* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all GPTs.
	 */
	static std::vector<Gpt*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Gpt*> get_all(const Devicegraph* devicegraph);

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
	virtual std::unique_ptr<Device> clone_v2() const override;

	Gpt(Impl* impl);
	ST_NO_SWIG Gpt(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Gpt.
     *
     * @throw NullPointerException
     */
    bool is_gpt(const Device* device);

    /**
     * Converts pointer to Device to pointer to Gpt.
     *
     * @return Pointer to Gpt.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Gpt* to_gpt(Device* device);

    /**
     * @copydoc to_gpt(Device*)
     */
    const Gpt* to_gpt(const Device* device);

}

#endif
