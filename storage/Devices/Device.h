/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2025] SUSE LLC
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
#include "storage/Utils/Swig.h"
#include "storage/View.h"


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


    /**
     * An abstract base class for storage devices. Storage devices are
     * nodes in the devicegraph.
     *
     * The Device class does not have a device name since some device
     * types do not have a intrinsic device name, e.g. btrfs. Instead
     * most devices are derived from BlkDevice which has a device
     * name, udev path and udev ids.
     *
     * @see Devicegraph
     */
    class Device : private boost::noncopyable
    {

    public:

	virtual ~Device();

	/**
	 * Return the storage id (sid) of the device.
	 *
	 * @see sid_t
	 */
	sid_t get_sid() const;

	bool operator==(const Device& rhs) const;
	bool operator!=(const Device& rhs) const;

	/**
	 * Copies the device to the devicegraph. Does not copy holders. The
	 * purpose of the function is to restore parts of the probed
	 * devicegraph in the staging devicegraph that were previously
	 * deleted.
	 *
	 * Device must not exist in devicegraph.
	 *
	 * @see Holder::copy_to_devicegraph()
	 *
	 * @throw Exception
	 */
	Device* copy_to_devicegraph(Devicegraph* devicegraph) const;

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

	/**
	 * Checks if the device exists in the system devicegraph.
	 */
	bool exists_in_system() const;

	std::string get_displayname() const;

	/**
	 * Detect the resize info of the device.
	 *
	 * If the device holds an NTFS it must not be mounted.
	 *
	 * @see ResizeInfo
	 *
	 * @throw Exception
	 */
	ResizeInfo detect_resize_info() const;

	bool has_children() const;
	size_t num_children() const;

	bool has_parents() const;
	size_t num_parents() const;

	/**
	 * Get all Devices.
	 */
	static std::vector<Device*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Device*> get_all(const Devicegraph* devicegraph);

	// TODO check if we can somehow return a iterator. getting rid of the
	// ptr would also allow to use references instead of pointer in the
	// interface.

	// all function without a view are obsolete (to be replaced by functions with
	// default view)

	std::vector<Device*> get_children();
	std::vector<const Device*> get_children() const;

	/**
	 * Get all children of the device.
	 */
	std::vector<Device*> get_children(View view);

	/**
	 * @copydoc get_children(View)
	 */
	std::vector<const Device*> get_children(View view) const;

	std::vector<Device*> get_parents();
	std::vector<const Device*> get_parents() const;

	/**
	 * Get all parents of the device.
	 */
	std::vector<Device*> get_parents(View view);

	/**
	 * @copydoc get_parents(View)
	 */
	std::vector<const Device*> get_parents(View view) const;

	std::vector<Device*> get_siblings(bool itself);
	std::vector<const Device*> get_siblings(bool itself) const;

	std::vector<Device*> get_descendants(bool itself);
	std::vector<const Device*> get_descendants(bool itself) const;

	std::vector<Device*> get_descendants(bool itself, View view);
	std::vector<const Device*> get_descendants(bool itself, View view) const;

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

	void remove_descendants() ST_DEPRECATED;

	/**
	 * Remove all descendants of the device. The view should likely always be
	 * View::REMOVE.
	 */
	void remove_descendants(View view);

	/**
	 * Return the userdata of the device.
	 */
	const std::map<std::string, std::string>& get_userdata() const;

	/**
	 * Set the userdata of the device.
	 */
	void set_userdata(const std::map<std::string, std::string>& userdata);

	friend std::ostream& operator<<(std::ostream& out, const Device& device);

	/**
	 * Compare (less than) two Devices by sid.
	 *
	 * @see get_sid()
	 */
	static bool compare_by_sid(const Device* lhs, const Device* rhs);

	/**
	 * Compare (less than) two Devices by name. So far only available for
	 * BlkDevices and LvmVgs.
	 *
	 * The comparison sorts to alphabetical order with some
	 * exceptions. E.g. The disk /dev/sdz is in front of /dev/sdaa and the
	 * partition /dev/sda2 is in front of /dev/sda10.
	 *
	 * The comparison fulfills strict weak ordering. The exact ordering
	 * may change in future versions.
	 *
	 * The comparison is locale unaware.
	 *
	 * @see get_name_sort_key()
	 *
	 * @throw Exception
	 */
	static bool compare_by_name(const Device* lhs, const Device* rhs);

	/**
	 * Get a sort-key based on the device name. So far only
	 * available for BlkDevices and LvmVgs. For other types the
	 * sort-key is empty.
	 *
	 * The sorting based on the sort-key sorts to alphabetical
	 * order with some exceptions. E.g. The disk /dev/sdz is in
	 * front of /dev/sdaa and the partition /dev/sda2 is in front
	 * of /dev/sda10.
	 *
	 * The sorting based on the sort-key may depend on the used
	 * locale.
	 *
	 * The sort-key and the exact ordering may change in future
	 * versions.
	 *
	 * @throw Exception
	 */
	std::string get_name_sort_key() const;

	/**
	 * Return the devicegraph the device belongs to.
	 */
	Devicegraph* get_devicegraph();

	/**
	 * @copydoc get_devicegraph()
	 */
	const Devicegraph* get_devicegraph() const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

	virtual Device* clone() const ST_DEPRECATED = 0;
	virtual std::unique_ptr<Device> clone_v2() const = 0;

	void save(xmlNode* node) const ST_DEPRECATED;

    protected:

	Device(Impl* impl) ST_DEPRECATED;
	ST_NO_SWIG Device(std::unique_ptr<Impl>&& impl);

	/**
	 * Create a device in the devicegraph.
	 *
	 * The created device is owned by the devicegraph.
	 */
	void create(Devicegraph* devicegraph) ST_DEPRECATED;

	void load(Devicegraph* devicegraph) ST_DEPRECATED;

    private:

	void add_to_devicegraph(Devicegraph* devicegraph) ST_DEPRECATED;

	const std::unique_ptr<Impl> impl;

    };

}

#endif
