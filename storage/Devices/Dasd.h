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


#ifndef STORAGE_DASD_H
#define STORAGE_DASD_H


#include "storage/Devices/Partitionable.h"


namespace storage
{

    enum class DasdType {
	UNKNOWN, ECKD, FBA
    };


    enum class DasdFormat {
	NONE, LDL, CDL
    };


    /**
     * A physical DASD device.
     */
    class Dasd : public Partitionable
    {
    public:

	static Dasd* create(Devicegraph* devicegraph, const std::string& name);
	static Dasd* create(Devicegraph* devicegraph, const std::string& name,
			    const Region& region);
	static Dasd* create(Devicegraph* devicegraph, const std::string& name,
			    unsigned long long size);

	static Dasd* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Sorted by name.
	 */
	static std::vector<Dasd*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Dasd*> get_all(const Devicegraph* devicegraph);

	bool is_rotational() const;

	DasdType get_dasd_type() const;

	DasdFormat get_dasd_format() const;

	static Dasd* find_by_name(Devicegraph* devicegraph, const std::string& name);
	static const Dasd* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Dasd* clone() const override;

    protected:

	Dasd(Impl* impl);

    };


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
