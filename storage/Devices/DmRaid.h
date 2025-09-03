/*
 * Copyright (c) [2017-2021] SUSE LLC
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


#ifndef STORAGE_DM_RAID_H
#define STORAGE_DM_RAID_H


#include "storage/Devices/Partitionable.h"


namespace storage
{


    /**
     * A DM RAID device
     */
    class DmRaid : public Partitionable
    {
    public:

	/**
	 * Create a device of type DmRaid. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static DmRaid* create(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static DmRaid* create(Devicegraph* devicegraph, const std::string& name,
			      const Region& region);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static DmRaid* create(Devicegraph* devicegraph, const std::string& name,
			      unsigned long long size);

	static DmRaid* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all DmRaids.
	 */
	static std::vector<DmRaid*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const DmRaid*> get_all(const Devicegraph* devicegraph);

	/**
	 * Return blk devices used for the DmRaid.
	 */
	std::vector<BlkDevice*> get_blk_devices();

	/**
	 * @copydoc get_blk_devices()
	 */
	std::vector<const BlkDevice*> get_blk_devices() const;

	/**
	 * Return whether the DM RAID is of rotational or
	 * non-rotational type. See the kernel documentation
	 * block/queue-sysfs.rst for more information.
	 */
	bool is_rotational() const;

	/**
	 * Find a DmRaid by its name. Only the name returned by get_name()
	 * is considered.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static DmRaid* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const DmRaid* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual DmRaid* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	DmRaid(Impl* impl);
	ST_NO_SWIG DmRaid(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a DmRaid.
     *
     * @throw NullPointerException
     */
    bool is_dm_raid(const Device* device);

    /**
     * Converts pointer to Device to pointer to DmRaid.
     *
     * @return Pointer to Dmraid.
     * @throw DeviceHasWrongType, NullPointerException
     */
    DmRaid* to_dm_raid(Device* device);

    /**
     * @copydoc to_dm_raid(Device*)
     */
    const DmRaid* to_dm_raid(const Device* device);

}

#endif
