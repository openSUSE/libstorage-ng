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


#ifndef STORAGE_BLK_DEVICE_H
#define STORAGE_BLK_DEVICE_H


#include <vector>

#include "storage/Utils/Swig.h"
#include "storage/Devices/Device.h"
#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    class Region;
    class Topology;
    class Encryption;
    enum class EncryptionType;
    class Bcache;
    class BcacheCset;
    class RemoveInfo;
    class SystemInfo;


    //! An abstract Block Device.
    class BlkDevice : public Device
    {
    public:

	/**
	 * Get the device name.
	 */
	const std::string& get_name() const;

	/**
	 * Set the device name.
	 *
	 * @throw Exception
	 */
	void set_name(const std::string& name);

	const Region& get_region() const;

	/**
	 * @throw Exception
	 */
	void set_region(const Region& region);

	/**
	 * Returns the size of the block device.
	 */
	unsigned long long get_size() const;

	/**
	 * Set the size of the block device.
	 *
	 * @see get_size()
	 * @throw Exception
	 */
	void set_size(unsigned long long size);

	/**
	 * Returns the size of the block device as a localised string.
	 *
	 * @see get_size(), byte_to_humanstring()
	 * @return size as string
	 */
	std::string get_size_string() const;

	/**
	 * Get the topology.
	 */
	const Topology& get_topology() const;

	/**
	 * Set the topology. Only useful for testsuites.
	 */
	void set_topology(const Topology& topology);

	bool is_active() const;

	/**
	 * Return whether the block device is read-only. E.g. LVM
	 * logical volumes can be read-only.
	 */
	bool is_read_only() const;

	const std::string& get_sysfs_name() const;
	const std::string& get_sysfs_path() const;

	/**
	 * Return the names of the udev by-path links of the blk
	 * device. The names do not include the directory. Usually
	 * there is only one by-path link.
	 *
	 * Example return value: [ "pci-0000:00:17.0-ata-1" ]
	 *
	 * This may not be the complete list of names known to udev
	 * since the library filters names that are known to cause
	 * problems.
	 */
	const std::vector<std::string>& get_udev_paths() const;

	/**
	 * Return the names of the udev by-id links of the blk
	 * device. The names do not include the directory.
	 *
	 * Example return value: [ "ata-SAMSUNG_SSD_PM871b_2.5_7mm_512GB_S3U3NE0K803507",
	 * "scsi-0ATA_SAMSUNG_SSD_PM87_S3U3NE0K803507" ]
	 *
	 * This may not be the complete list of names known to udev
	 * since the library filters names that are known to cause
	 * problems.
	 */
	const std::vector<std::string>& get_udev_ids() const;

	/**
	 * Checks whether the blk device is in general usable as a blk
	 * device. This is not the case for some DASDs, see
	 * doc/dasd.md, extended partitions, LVM thin and cache pools,
	 * MD RAID containers, see doc/md-raid.md and host-managed
	 * zoned disks.
	 *
	 * Does not consider if the blk device is already in use.
	 */
	bool is_usable_as_blk_device() const;

	/**
	 * Check whether the device can be removed. E.g. disks and DASDs cannot be
	 * removed.
	 *
	 * @see RemoveInfo
	 *
	 * @throw Exception
	 */
	RemoveInfo detect_remove_info() const;

	/**
	 * Return device-mapper table name (dm-table-name for short). Empty if
	 * this is not a device-mapper device.
	 */
	const std::string& get_dm_table_name() const;

	/**
	 * Set the device-mapper table name (dm-table-name for short).
	 *
	 * @see get_dm_table_name()
	 */
	void set_dm_table_name(const std::string& dm_table_name);

	/**
	 * Check whether the DM table name is valid. Does not check for collisions.
	 */
	static bool is_valid_dm_table_name(const std::string& dm_table_name);

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
	 * Check if a block device by any name including any symbolic links in
	 * /dev. Function might require a system lookup and is therefore
	 * slow. Only works on the probed devicegraph.
	 *
	 * @throw Exception
	 *
	 */
	static bool exists_by_any_name(const Devicegraph* devicegraph, const std::string& name,
				       SystemInfo& system_info);

	/**
	 * Find a block device by any name including any symbolic links in
	 * /dev. Function might require a system lookup and is therefore
	 * slow. Only works on the probed devicegraph.
	 *
	 * @throw DeviceNotFoundByName, DeviceHasWrongType, Exception
	 */
	static BlkDevice* find_by_any_name(Devicegraph* devicegraph, const std::string& name) ST_DEPRECATED;

	/**
	 * Find a block device by any name including any symbolic links in
	 * /dev. Function might require a system lookup and is therefore
	 * slow. Only works on the probed devicegraph.
	 *
	 * @throw DeviceNotFoundByName, DeviceHasWrongType, Exception
	 */
	static BlkDevice* find_by_any_name(Devicegraph* devicegraph, const std::string& name,
					   SystemInfo& system_info);

	/**
	 * @copydoc find_by_any_name
	 */
	static const BlkDevice* find_by_any_name(const Devicegraph* devicegraph, const std::string& name) ST_DEPRECATED;

	/**
	 * @copydoc find_by_any_name
	 */
	static const BlkDevice* find_by_any_name(const Devicegraph* devicegraph, const std::string& name,
						 SystemInfo& system_info);

	/**
	 * Creates a block filesystem on the block device.
	 *
	 * @throw WrongNumberOfChildren, UnsupportedException
	 */
	BlkFilesystem* create_blk_filesystem(FsType fs_type);

	/**
	 * Return whether the block device has a block filesystem.
	 */
	bool has_blk_filesystem() const;

	/**
	 * Return the block filesystem of the block device.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	BlkFilesystem* get_blk_filesystem();

	/**
	 * @copydoc get_blk_filesystem
	 */
	const BlkFilesystem* get_blk_filesystem() const;

	/**
	 * @copydoc create_blk_filesystem
	 */
	BlkFilesystem* create_filesystem(FsType fs_type) ST_DEPRECATED { return create_blk_filesystem(fs_type); }

	/**
	 * @copydoc has_blk_filesystem
	 */
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
	 */
	Encryption* create_encryption(const std::string& dm_table_name) ST_DEPRECATED;

	/**
	 * Creates an encryption device on the blk device. If the blk device
	 * has children the children will become children of the encryption
	 * device.
	 *
	 * Allowed encryption types are LUKS1, LUKS2 and PLAIN.
	 *
	 * It will also set the mount-by method of the encryption to the
	 * storage default mount-by method.
	 *
	 * @see EncryptionType
	 */
	Encryption* create_encryption(const std::string& dm_table_name, EncryptionType type);

	/**
	 * Removes an encryption device on the blk device. If the encryption device
	 * has children the children will become children of the blk device.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	void remove_encryption();

	/**
	 * Return whether the block device has an Encryption.
	 */
	bool has_encryption() const;

	/**
	 * Return the Encryption of the block device.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	Encryption* get_encryption();

	/**
	 * @copydoc get_encryption
	 */
	const Encryption* get_encryption() const;

	/**
	 * Creates a Bcache on the blk device. If the blk device has children
	 * the children will become children of the bcache device.
	 */
	Bcache* create_bcache(const std::string& name);

	/**
	 * Return whether the block device has a BCache.
	 */
	bool has_bcache() const;

	/**
	 * Return the Bcache of the block device.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	Bcache* get_bcache();

	/**
	 * @copydoc get_bcache()
	 */
	const Bcache* get_bcache() const;

	/**
	 * Creates a BcacheCset on the blk device.
	 *
	 * @throw WrongNumberOfChildren
	 */
	BcacheCset* create_bcache_cset();

	/**
	 * Return whether the block device has a BcacheCset.
	 */
	bool has_bcache_cset() const;

	/**
	 * Return the BcacheCset of the block device.
	 *
	 * @throw WrongNumberOfChildren, DeviceHasWrongType
	 */
	BcacheCset* get_bcache_cset();

	/**
	 * @copydoc get_bcache_cset()
	 */
	const BcacheCset* get_bcache_cset() const;

	/**
	 * Compare (less than) two BlkDevices by DM table name.
	 *
	 * The comparison is locale unaware.
	 *
	 * @see get_dm_table_name()
	 */
	static bool compare_by_dm_table_name(const BlkDevice* lhs, const BlkDevice* rhs);

	/**
	 * Returns the possible mount-by methods to reference the block device.
	 *
	 * PARTLABEL is included even if the partition label is not set.
	 */
	std::vector<MountByType> possible_mount_bys() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	BlkDevice(Impl* impl);
	ST_NO_SWIG BlkDevice(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an BlkDevice.
     *
     * @throw NullPointerException
     */
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
