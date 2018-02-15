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


#ifndef STORAGE_BLK_DEVICE_H
#define STORAGE_BLK_DEVICE_H


#include "storage/Utils/Swig.h"
#include "storage/Devices/Device.h"
#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    class Region;
    class Encryption;


    //! An abstract Block Device.
    class BlkDevice : public Device
    {
    public:

	const std::string& get_name() const;
	void set_name(const std::string& name);

	const Region& get_region() const;

	/**
	 * @throw Exception
	 */
	void set_region(const Region& region);

	unsigned long long get_size() const;

	/**
	 * @throw Exception
	 */
	void set_size(unsigned long long size);

	std::string get_size_string() const;

	const std::string& get_sysfs_name() const;
	const std::string& get_sysfs_path() const;

	const std::vector<std::string>& get_udev_paths() const;
	const std::vector<std::string>& get_udev_ids() const;

	/**
	 * Checks whether the blk device is in general usable as a blk
	 * device. This is not the case for some DASDs, see doc/dasd.md,
	 * extended partitions, LVM thin pools and MD RAID containers, see
	 * doc/md-raid.md.
	 *
	 * Does not consider if the blk device is already in use.
	 */
	bool is_usable_as_blk_device() const;

	/**
	 * Return device-mapper table name (dm-table-name for short). Empty if
	 * this is not a device-mapper device.
	 */
	const std::string& get_dm_table_name() const;

	void set_dm_table_name(const std::string& dm_table_name);

	/**
	 * Get all BlkDevices.
	 */
	static std::vector<BlkDevice*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const BlkDevice*> get_all(const Devicegraph* devicegraph);

	/**
	 * Find a block device by its name. Only the name returned by
	 * get_name() is considered.
	 *
	 * @throw DeviceNotFoundByName, DeviceHasWrongType
	 */
	static BlkDevice* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const BlkDevice* find_by_name(const Devicegraph* devicegraph, const std::string& name);

	/**
	 * Find a block device by any name including any symbolic links in
	 * /dev. Function might require a system lookup and is therefore
	 * slow. Only works on the probed devicegraph.
	 *
	 * @throw DeviceNotFoundByName, DeviceHasWrongType, Exception
	 */
	static BlkDevice* find_by_any_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_any_name
	 */
	static const BlkDevice* find_by_any_name(const Devicegraph* devicegraph, const std::string& name);

	/**
	 * Creates a block filesystem on the block device.
	 *
	 * @throw WrongNumberOfChildren, UnsupportedException
	 */
	BlkFilesystem* create_blk_filesystem(FsType fs_type);

	bool has_blk_filesystem() const;

	/**
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	BlkFilesystem* get_blk_filesystem();

	/**
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	const BlkFilesystem* get_blk_filesystem() const;

	/**
	 * @copydoc create_blk_filesystem
	 */
	BlkFilesystem* create_filesystem(FsType fs_type) ST_DEPRECATED { return create_blk_filesystem(fs_type); }

	bool has_filesystem() const ST_DEPRECATED { return has_blk_filesystem(); }

	/**
	 * @copydoc get_blk_filesystem
	 */
	BlkFilesystem* get_filesystem() ST_DEPRECATED { return get_blk_filesystem(); }

	/**
	 * @copydoc get_blk_filesystem
	 */
	const BlkFilesystem* get_filesystem() const ST_DEPRECATED { return get_blk_filesystem(); }

	/**
	 * Creates an encryption device on the blk device. If the blk device
	 * has children the children will become children of the encryption
	 * device.
	 *
	 * It will also set the mount-by method of the encryption to the
	 * storage default mount-by method.
	 *
	 * TODO parameter for encryption type? do all encryptions need a dm_name?
	 */
	Encryption* create_encryption(const std::string& dm_name);

	/**
	 * Removes an encryption device on the blk device. If the encryption device
	 * has children the children will become children of the blk device.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	void remove_encryption();

	bool has_encryption() const;

	/**
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	Encryption* get_encryption();

	/**
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	const Encryption* get_encryption() const;

	/**
	 * Compare (less than) two BlkDevices by DM table name.
	 */
	static bool compare_by_dm_table_name(const BlkDevice* lhs, const BlkDevice* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	BlkDevice(Impl* impl);

    };


    bool is_blk_device(const Device* device);

    /**
     * Converts pointer to Device to pointer to BlkDevice.
     *
     * @return Pointer to BlkDevice.
     * @throw DeviceHasWrongType, NullPointerException
     */
    BlkDevice* to_blk_device(Device* device);

    /**
     * @copydoc to_blk_device(Device*)
     */
    const BlkDevice* to_blk_device(const Device* device);

}

#endif
