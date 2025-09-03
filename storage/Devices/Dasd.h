/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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


#ifndef STORAGE_DASD_H
#define STORAGE_DASD_H


#include "storage/Devices/Partitionable.h"


namespace storage
{

    /**
     * The DASD type.
     */
    enum class DasdType {
	UNKNOWN, ECKD, FBA
    };


    /**
     * The layout with which the ECKD DASD was formatted.
     */
    enum class DasdFormat {
	NONE, LDL, CDL
    };


    /**
     * Convert the DASD type dasd_type to a string.
     *
     * @see DasdType
     */
    std::string get_dasd_type_name(DasdType dasd_type);


    /**
     * Convert the DASD format dasd_format to a string.
     *
     * @see DasdFormat
     */
    std::string get_dasd_format_name(DasdFormat dasd_format);


    /**
     * A physical DASD device.
     */
    class Dasd : public Partitionable
    {
    public:

	/**
	 * Create a device of type Dasd. Usually this function is not called directly. It
	 * is called during probing.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Dasd* create(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static Dasd* create(Devicegraph* devicegraph, const std::string& name,
			    const Region& region);

	/**
	 * @copydoc create(Devicegraph*, const std::string&)
	 */
	static Dasd* create(Devicegraph* devicegraph, const std::string& name,
			    unsigned long long size);

	static Dasd* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Dasds.
	 */
	static std::vector<Dasd*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Dasd*> get_all(const Devicegraph* devicegraph);

	/**
	 * Get the bus ID of the DASD. E.g. "0.0.0150".
	 */
	std::string get_bus_id() const;

	/**
	 * Set the bus ID of the DASD.
	 *
	 * @note The bus ID cannot be changed on disk. The function is only for
	 * testsuites.
	 *
	 * @see get_bus_id()
	 */
	void set_bus_id(std::string bus_id);

	/**
	 * Return whether the DASD is of rotational or non-rotational
	 * type. See the kernel documentation block/queue-sysfs.rst
	 * for more information.
	 */
	bool is_rotational() const;

	/**
	 * Get the DASD type.
	 *
	 * @see DasdType
	 */
	DasdType get_type() const;

	/**
	 * Set the DASD type.
	 *
	 * @note The library does not support to change the type on
	 * disk. The function is only for testsuites.
	 *
	 * @see DasdType
	 */
	void set_type(DasdType type);

	/**
	 * Get the DASD format.
	 *
	 * @see DasdFormat
	 */
	DasdFormat get_format() const;

	/**
	 * Set the DASD format.
	 *
	 * @note The library does not support to change the format on
	 * disk. The function is only for testsuites.
	 *
	 * @see DasdFormat
	 */
	void set_format(DasdFormat format);

	/**
	 * Find a Dasd by its name. Only the name returned by get_name() is
	 * considered.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Dasd* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const Dasd* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Dasd* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Dasd(Impl* impl);
	ST_NO_SWIG Dasd(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Dasd.
     *
     * @throw NullPointerException
     */
    bool is_dasd(const Device* device);

    /**
     * Converts pointer to Device to pointer to Dasd.
     *
     * @return Pointer to Dasd.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Dasd* to_dasd(Device* device);

    /**
     * @copydoc to_dasd(Device*)
     */
    const Dasd* to_dasd(const Device* device);

}

#endif
