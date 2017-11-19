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


#ifndef STORAGE_DEVICE_H
#define STORAGE_DEVICE_H


#include <libxml/tree.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <boost/noncopyable.hpp>

#include "storage/Utils/Exception.h"


//! The storage namespace.
namespace storage
{
    class Devicegraph;
    class Holder;
    class ResizeInfo;


    class DeviceHasWrongType : public Exception
    {
    public:

	DeviceHasWrongType(const char* seen, const char* expected);
    };


    //! An integer storage ID.
    /// The storage id (sid) is used to identify devices.  The sid is copied
    /// when copying the device graph.  When adding a device it gets a unique
    /// sid (across all device graphs).  By using the storage id instead of a
    /// device name we can easily identify devices where the device name
    /// changed, e.g. renumbered logical partitions or renamed logical volumes.
    /// Also some devices do not have a intrinsic device name, e.g. btrfs.  We
    /// could even have objects for unused space, e.g. space between partitions
    /// or unallocated space of a volume group.
    ///
    /// Whenever possible use the storage id to find objects instead of
    /// e.g. the name since only the storage id guarantees uniqueness.
    typedef unsigned int sid_t;


    //! An abstract base class of storage devices,
    //! and a vertex in the Devicegraph.
    /// The Device class does not have a device name since some device types do
    /// not have a intrinsic device name, e.g. btrfs.  Instead most devices are
    /// derived from BlkDevice which has a device name, udev path and udev ids.

    class Device : private boost::noncopyable
    {

    public:

	virtual ~Device();

	sid_t get_sid() const;

	bool operator==(const Device& rhs) const;
	bool operator!=(const Device& rhs) const;

	/**
	 * Checks if the device exists in the devicegraph.
	 */
	bool exists_in_devicegraph(const Devicegraph* devicegraph) const;

	/**
	 * Checks if the device exists in the probed devicegraph.
	 */
	bool exists_in_probed() const;

	/**
	 * Checks if the device exists in the staging devicegraph.
	 */
	bool exists_in_staging() const;

	std::string get_displayname() const;

	ResizeInfo detect_resize_info() const;

	bool has_children() const;
	size_t num_children() const;

	bool has_parents() const;
	size_t num_parents() const;

	static std::vector<Device*> get_all(Devicegraph* devicegraph);
	static std::vector<const Device*> get_all(const Devicegraph* devicegraph);

	// TODO check if we can somehow return a iterator. getting rid of the
	// ptr would also allow to use references instead of pointer in the
	// interface.

	std::vector<Device*> get_children();
	std::vector<const Device*> get_children() const;

	std::vector<Device*> get_parents();
	std::vector<const Device*> get_parents() const;

	std::vector<Device*> get_siblings(bool itself);
	std::vector<const Device*> get_siblings(bool itself) const;

	std::vector<Device*> get_descendants(bool itself);
	std::vector<const Device*> get_descendants(bool itself) const;

	std::vector<Device*> get_ancestors(bool itself);
	std::vector<const Device*> get_ancestors(bool itself) const;

	std::vector<Device*> get_leaves(bool itself);
	std::vector<const Device*> get_leaves(bool itself) const;

	std::vector<Device*> get_roots(bool itself);
	std::vector<const Device*> get_roots(bool itself) const;

	std::vector<Holder*> get_in_holders();
	std::vector<const Holder*> get_in_holders() const;

	std::vector<Holder*> get_out_holders();
	std::vector<const Holder*> get_out_holders() const;

	void remove_descendants();

	const std::map<std::string, std::string>& get_userdata() const;
	void set_userdata(const std::map<std::string, std::string>& userdata);

	friend std::ostream& operator<<(std::ostream& out, const Device& device);

	/**
	 * Compare (less than) two Devices by sid.
	 */
	static bool compare_by_sid(const Device* lhs, const Device* rhs);

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

	virtual Device* clone() const = 0;

	void save(xmlNode* node) const;

    protected:

	Device(Impl* impl);

	void create(Devicegraph* devicegraph);
	void load(Devicegraph* devicegraph);

    private:

	void add_to_devicegraph(Devicegraph* devicegraph);

	const std::unique_ptr<Impl> impl;

    };

}

#endif
