/*
 * Copyright (c) 2017 SUSE LLC
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


    //! A multipath device
    class Multipath : public Partitionable
    {
    public:

	static Multipath* create(Devicegraph* devicegraph, const std::string& name);
	static Multipath* create(Devicegraph* devicegraph, const std::string& name,
				 const Region& region);
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

	const std::string& get_vendor() const;

	const std::string& get_model() const;

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

	/**
	 * Compare (less than) two Multipaths by name.
	 */
	static bool compare_by_name(const Multipath* lhs, const Multipath* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Multipath* clone() const override;

    protected:

	Multipath(Impl* impl);

    };


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
