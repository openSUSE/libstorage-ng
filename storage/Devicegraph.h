/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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


#ifndef STORAGE_DEVICEGRAPH_H
#define STORAGE_DEVICEGRAPH_H


#include <stdexcept>
#include <boost/noncopyable.hpp>

#include "storage/Devices/Device.h"
#include "storage/Graphviz.h"


namespace storage
{
    class Storage;
    class Device;
    class Holder;
    class Disk;
    class Md;
    class LvmVg;
    class Filesystem;
    class BlkFilesystem;
    class CheckCallbacks;


    class DeviceNotFound : public Exception
    {
    public:

	DeviceNotFound(const std::string& msg);
    };


    class DeviceNotFoundBySid : public DeviceNotFound
    {
    public:

	DeviceNotFoundBySid(sid_t sid);
    };


    class DeviceNotFoundByName : public DeviceNotFound
    {
    public:

	DeviceNotFoundByName(const std::string& name);
    };


    class DeviceNotFoundByUuid : public DeviceNotFound
    {
    public:

	DeviceNotFoundByUuid(const std::string& uuid);
    };


    class HolderNotFound : public Exception
    {
    public:

	HolderNotFound(const std::string& msg);
    };


    class HolderNotFoundBySids : public HolderNotFound
    {
    public:

	HolderNotFoundBySids(sid_t source_sid, sid_t target_sid);
    };


    class HolderAlreadyExists : public Exception
    {
    public:

	HolderAlreadyExists(sid_t source_sid, sid_t target_sid);
    };


    class WrongNumberOfParents : public Exception
    {
    public:

	WrongNumberOfParents(size_t seen, size_t expected);
    };


    class WrongNumberOfChildren : public Exception
    {
    public:

	WrongNumberOfChildren(size_t seen, size_t expected);
    };


    /**
     * The master container of the libstorage.
     *
     * There are two levels of functions to manipulate the device graph. As an
     * example we show how to create a partition table containing one
     * partition on the disk sda.
     *
     * - High-level
     *     \parblock
     *     \code{.cpp}
     *     Gpt* gpt = sda->create_partition_table(GPT);
     *     Partition* sda1 = gpt->create_partition("/dev/sda1", Region(0, 100, sector-size), PRIMARY)
     *     \endcode
     *
     *     These functions will not only create the Gpt and Partition object but
     *     also the holders in the device graph.
     *     \endparblock
     *
     * - Low-level
     *     \parblock
     *     \code{.cpp}
     *     Gpt* gpt = Gpt::create(staging);
     *     User::create(staging, sda, gpt);
     *
     *     Partition* sda1 = Partition::create(staging, "/dev/sda1", Region(0, 100, sector-size), PRIMARY);
     *     Subdevice::create(staging, gpt, sda1);
     *     \endcode
     *
     *     As you can see with the low-level functions you have to create the holders yourself.
     *     \endparblock
     *
     * Whenever possible use the high-level functions.
     */
    class Devicegraph : private boost::noncopyable
    {

    public:

	Devicegraph(Storage* storage);
	~Devicegraph();

	bool operator==(const Devicegraph& rhs) const;
	bool operator!=(const Devicegraph& rhs) const;

	Storage* get_storage();
	const Storage* get_storage() const;

	/**
	 * @throw Exception
	 */
	void load(const std::string& filename);

	/**
	 * @throw Exception
	 */
	void save(const std::string& filename) const;

	bool empty() const;

	size_t num_devices() const;
	size_t num_holders() const;

	/**
	 * @throw DeviceNotFoundBySid
	 */
	Device* find_device(sid_t sid);

	/**
	 * @throw DeviceNotFoundBySid
	 */
	const Device* find_device(sid_t sid) const;

	/**
	 * Check whether the device with sid exists.
	 */
	bool device_exists(sid_t sid) const;

	/**
	 * Check whether the holder with source sid and target sid exists.
	 */
	bool holder_exists(sid_t source_sid, sid_t target_sid) const;

	void clear();

	// convenient functions, equivalent to e.g. Disk::get_all(devicegraph)
	std::vector<Disk*> get_all_disks();
	std::vector<const Disk*> get_all_disks() const;

	std::vector<Md*> get_all_mds();
	std::vector<const Md*> get_all_mds() const;

	std::vector<LvmVg*> get_all_lvm_vgs();
	std::vector<const LvmVg*> get_all_lvm_vgs() const;

	std::vector<Filesystem*> get_all_filesystems();
	std::vector<const Filesystem*> get_all_filesystems() const;

	std::vector<BlkFilesystem*> get_all_blk_filesystems();
	std::vector<const BlkFilesystem*> get_all_blk_filesystems() const;

	/**
	 * Removes the device with sid from the devicegraph. Only use this
	 * function if there is no special function to delete a device,
	 * e.g. PartitionTable.delete_partition() or LvmVg.delete_lvm_lv().
	 *
	 * @throw DeviceNotFoundBySid
	 *
	 * TODO internally redirect to special delete functions?
	 */
	void remove_device(sid_t sid);

	void remove_device(Device* a);

	void remove_devices(std::vector<Device*> devices);

	/**
	 * @throw HolderNotFoundBySids
	 */
	Holder* find_holder(sid_t source_sid, sid_t target_sid);

	/**
	 * @throw HolderNotFoundBySids
	 */
	const Holder* find_holder(sid_t source_sid, sid_t target_sid) const;

	void remove_holder(Holder* holder);

	/**
	 * Checks the devicegraph.
	 *
	 * See also Storage::check().
	 *
	 * @throw Exception
	 */
	void check(const CheckCallbacks* check_callbacks = nullptr) const;

	/**
	 * Calculates a bit-field with the used features of the devicegraph.
	 */
	uint64_t used_features() const;

	// TODO move to Impl
	void copy(Devicegraph& dest) const;

	/**
	 * Writes the devicegraph in graphviz format. The node id is the sid
	 * (storage id).
	 *
	 * @throw Exception
	 */
	void write_graphviz(const std::string& filename, GraphvizFlags flags = GraphvizFlags::NAME,
			    GraphvizFlags tooltip_flags = GraphvizFlags::NONE) const;

	friend std::ostream& operator<<(std::ostream& out, const Devicegraph& devicegraph);

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}

#endif
