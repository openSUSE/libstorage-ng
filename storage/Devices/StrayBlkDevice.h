/*
 * Copyright (c) [2018-2020] SUSE LLC
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


#ifndef STORAGE_STRAY_BLK_DEVICE_H
#define STORAGE_STRAY_BLK_DEVICE_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    /**
     * This class represents stray block devices. Stray block devices
     * have no parent in the devicegraph and cannot be partitioned.
     *
     * Normally only Disks and Dasds have no parents in the
     * devicegraph but Disks and Dasds can be partitioned.
     *
     * Currently only Xen Virtual Partitions are represented as stray
     * block devices (e.g. /dev/xvda1). Note that partitionable
     * devices in Xen are represented as Disks (e.g. /dev/xvda) and
     * partitions on them as normal Partitions (e.g. also /dev/xvda1).
     *
     * Note: If YaST supports filesystems directly on disks this class
     * might be obsolete and be removed.
     */
    class StrayBlkDevice : public BlkDevice
    {
    public:

	/**
	 * Create a device of type StrayBlkDevice. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static StrayBlkDevice* create(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static StrayBlkDevice* create(Devicegraph* devicegraph, const std::string& name,
				      const Region& region);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static StrayBlkDevice* create(Devicegraph* devicegraph, const std::string& name,
				      unsigned long long size);

	static StrayBlkDevice* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all StrayBlkDevices.
	 */
	static std::vector<StrayBlkDevice*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const StrayBlkDevice*> get_all(const Devicegraph* devicegraph);

	/**
	 * Find a StrayBlkDevice by its name. Only the name returned by get_name() is
	 * considered.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static StrayBlkDevice* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const StrayBlkDevice* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual StrayBlkDevice* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	StrayBlkDevice(Impl* impl);
	ST_NO_SWIG StrayBlkDevice(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a StrayBlkDevice.
     *
     * @throw NullPointerException
     */
    bool is_stray_blk_device(const Device* device);

    /**
     * Converts pointer to Device to pointer to StrayBlkDevice.
     *
     * @return Pointer to StrayBlkDevice.
     * @throw DeviceHasWrongType, NullPointerException
     */
    StrayBlkDevice* to_stray_blk_device(Device* device);

    /**
     * @copydoc to_stray_blk_device(Device*)
     */
    const StrayBlkDevice* to_stray_blk_device(const Device* device);

}

#endif
