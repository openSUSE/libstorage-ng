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


#ifndef STORAGE_MSDOS_H
#define STORAGE_MSDOS_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    class Msdos : public PartitionTable
    {
    public:

	/**
	 * Create a device of type Msdos. Usually this function is not called
	 * directly. Instead Partitionable::create_partition_table() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Msdos* create(Devicegraph* devicegraph);

	static Msdos* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Msdos* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	/**
	 * Get the size of the MBR gap (see
	 * https://en.wikipedia.org/wiki/BIOS_boot_partition). Here
	 * the MBR gap includes the 512 bytes for the MBR itself.
	 *
	 * @see set_minimal_mbr_gap()
	 */
	unsigned long get_minimal_mbr_gap() const;

	/**
	 * Set the size of the MBR gap.
	 *
	 * @see get_minimal_mbr_gap()
	 */
	void set_minimal_mbr_gap(unsigned long minimal_mbr_gap);

	Msdos(Impl* impl);
	ST_NO_SWIG Msdos(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Msdos.
     *
     * @throw NullPointerException
     */
    bool is_msdos(const Device* device);

    /**
     * Converts pointer to Device to pointer to Msdos.
     *
     * @return Pointer to Msdos.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Msdos* to_msdos(Device* device);

    /**
     * @copydoc to_msdos(Device*)
     */
    const Msdos* to_msdos(const Device* device);

}


#endif
