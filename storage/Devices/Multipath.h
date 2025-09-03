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


#ifndef STORAGE_MULTIPATH_H
#define STORAGE_MULTIPATH_H


#include "storage/Devices/Partitionable.h"


namespace storage
{


    /**
     * A multipath device. Not for native NVMe multipath.
     */
    class Multipath : public Partitionable
    {
    public:

	/**
	 * Create a device of type Multipath. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Multipath* create(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static Multipath* create(Devicegraph* devicegraph, const std::string& name,
				 const Region& region);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static Multipath* create(Devicegraph* devicegraph, const std::string& name,
				 unsigned long long size);

	static Multipath* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Multipaths.
	 */
	static std::vector<Multipath*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Multipath*> get_all(const Devicegraph* devicegraph);

	/**
	 * The vendor as reported by multipath tools. E.g. "IBM".
	 *
	 * @see get_model()
	 */
	const std::string& get_vendor() const;

	/**
	 * The model as reported by multipath tools. E.g. "2107900".
	 *
	 * @see get_vendor()
	 */
	const std::string& get_model() const;

	/**
	 * Return blk devices used for the Multipath.
	 */
	std::vector<BlkDevice*> get_blk_devices();

	/**
	 * @copydoc get_blk_devices()
	 */
	std::vector<const BlkDevice*> get_blk_devices() const;

	/**
	 * Return whether the multipath device is of rotational or
	 * non-rotational type. See the kernel documentation
	 * block/queue-sysfs.rst for more information.
	 */
	bool is_rotational() const;

	/**
	 * Find a Multipath by its name. Only the name returned by get_name()
	 * is considered.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Multipath* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const Multipath* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Multipath* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Multipath(Impl* impl);
	ST_NO_SWIG Multipath(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Multipath.
     *
     * @throw NullPointerException
     */
    bool is_multipath(const Device* device);

    /**
     * Converts pointer to Device to pointer to Multipath.
     *
     * @return Pointer to Multipath.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Multipath* to_multipath(Device* device);

    /**
     * @copydoc to_multipath(Device*)
     */
    const Multipath* to_multipath(const Device* device);

}

#endif
