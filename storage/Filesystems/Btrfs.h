/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2017-2020] SUSE LLC
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


#ifndef STORAGE_BTRFS_H
#define STORAGE_BTRFS_H


#include "storage/Devicegraph.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/BtrfsQgroup.h"


namespace storage
{

    class BtrfsSubvolume;
    class FilesystemUser;


    /**
     * Btrfs RAID levels (aka profiles) used for metadata and data.
     *
     * @see https://btrfs.wiki.kernel.org/index.php/Using_Btrfs_with_Multiple_Devices
     */
    enum class BtrfsRaidLevel
    {
	UNKNOWN, DEFAULT, SINGLE, DUP, RAID0, RAID1, RAID5, RAID6, RAID10, RAID1C3, RAID1C4
    };


    /**
     * Convert the btrfs RAID level btrfs_raid_level to a string.
     *
     * @see BtrfsRaidLevel
     */
    std::string get_btrfs_raid_level_name(BtrfsRaidLevel btrfs_raid_level);


    class BtrfsSubvolumeNotFoundByPath : public DeviceNotFound
    {
    public:

	BtrfsSubvolumeNotFoundByPath(const std::string& path);
    };


    class BtrfsQgroupNotFoundById : public DeviceNotFound
    {
    public:

	BtrfsQgroupNotFoundById(const BtrfsQgroup::id_t& id);
    };


    /**
     * Class to represent a btrfs filesystem
     * https://en.wikipedia.org/wiki/Btrfs in the devicegraph.
     */
    class Btrfs : public BlkFilesystem
    {
    public:

	/**
	 * Create a device of type Btrfs. Usually this function is not called
	 * directly. Instead BlkDevice::create_blk_filesystem() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Btrfs* create(Devicegraph* devicegraph);

	static Btrfs* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the metadata RAID level.
	 *
	 * @see BtrfsRaidLevel
	 */
	BtrfsRaidLevel get_metadata_raid_level() const;

	/**
	 * Set the metadata RAID level.
	 *
	 * Not supported for btrfs already existing on disk.
	 *
	 * @see BtrfsRaidLevel
	 */
	void set_metadata_raid_level(BtrfsRaidLevel metadata_raid_level);

	/**
	 * Get the data RAID level.
	 *
	 * @see BtrfsRaidLevel
	 */
	BtrfsRaidLevel get_data_raid_level() const;

	/**
	 * Set the data RAID level.
	 *
	 * Not supported for btrfs already existing on disk.
	 *
	 * @see BtrfsRaidLevel
	 */
	void set_data_raid_level(BtrfsRaidLevel data_raid_level);

	/**
	 * Get the allowed metadata RAID levels for the btrfs. So far,
	 * this depends on the number of devices. Levels for which
	 * mkfs.btrfs warns that they are not recommended are not
	 * included here. Additionally DEFAULT is allowed when creating
	 * a btrfs.
	 *
	 * @see BtrfsRaidLevel
	 */
	std::vector<BtrfsRaidLevel> get_allowed_metadata_raid_levels() const;

	/**
	 * Get the allowed data RAID levels for the btrfs. So far,
	 * this depends on the number of devices. Levels for which
	 * mkfs.btrfs warns that they are not recommended are not
	 * included here. Additionally DEFAULT is allowed when creating
	 * a btrfs.
	 *
	 * @see BtrfsRaidLevel
	 */
	std::vector<BtrfsRaidLevel> get_allowed_data_raid_levels() const;

	/**
	 * Return whether quota is enabled for the btrfs.
	 */
	bool has_quota() const;

	/**
	 * Enable or disable quota for the btrfs.
	 *
	 * When enabling quota, qgroups and qgroup relations are created for the
	 * btrfs. This is done so that no qgroup related actions will be done during
	 * commit (unless further changes are done). If quota was disabled during probing,
	 * the qgroups are created like btrfs would do. If quota was enabled during
	 * probing, the qgroups from probing are restored.
	 *
	 * When disabling quota, all qgroups and qgroup relations of the btrfs are
	 * removed.
	 *
	 * @throw Exception
	 */
	void set_quota(bool quota);

	/**
	 * Add a block device to the btrfs.
	 *
	 * @throw WrongNumberOfChildren
	 */
	FilesystemUser* add_device(BlkDevice* blk_device);

	/**
	 * Remove a block device from the btrfs.
	 *
	 * Not supported for btrfs already existing on disk.
	 *
	 * @throw Exception
	 */
	void remove_device(BlkDevice* blk_device);

	/**
	 * Get the top-level subvolume.
	 */
	BtrfsSubvolume* get_top_level_btrfs_subvolume();

	/**
	 * @copydoc get_top_level_btrfs_subvolume()
	 */
	const BtrfsSubvolume* get_top_level_btrfs_subvolume() const;

	/**
	 * Get the default subvolume.
	 */
	BtrfsSubvolume* get_default_btrfs_subvolume();

	/**
	 * @copydoc get_default_btrfs_subvolume()
	 */
	const BtrfsSubvolume* get_default_btrfs_subvolume() const;

	/**
	 * Set the default subvolume.
	 *
	 * @see get_default_btrfs_subvolume()
	 */
	void set_default_btrfs_subvolume(BtrfsSubvolume* btrfs_subvolume) const;

	/**
	 * Get all subvolumes of the btrfs.
	 */
	std::vector<BtrfsSubvolume*> get_btrfs_subvolumes();

	/**
	 * @copydoc get_btrfs_subvolumes()
	 */
	std::vector<const BtrfsSubvolume*> get_btrfs_subvolumes() const;

	/**
	 * Find a btrfs subvolume of the btrfs by its path.
	 *
	 * @throw BtrfsSubvolumeNotFoundByPath
	 */
	BtrfsSubvolume* find_btrfs_subvolume_by_path(const std::string& path);

	/**
	 * @copydoc find_btrfs_subvolume_by_path(const std::string&)
	 */
	const BtrfsSubvolume* find_btrfs_subvolume_by_path(const std::string& path) const;

	/**
	 * Create a new btrfs qgroup on the btrfs. Creating level 0 qgroups is not
	 * supported. Quota must be enabled.
	 *
	 * @throw Exception
	 */
	BtrfsQgroup* create_btrfs_qgroup(const BtrfsQgroup::id_t& id);

	/**
	 * Get all btrfs qgroups of the btrfs.
	 */
	std::vector<BtrfsQgroup*> get_btrfs_qgroups();

	/**
	 * @copydoc get_btrfs_qgroups()
	 */
	std::vector<const BtrfsQgroup*> get_btrfs_qgroups() const;

	/**
	 * Find a btrfs qgroup of the btrfs by its id.
	 *
	 * @throw BtrfsQgroupNotFoundById
	 */
	BtrfsQgroup* find_btrfs_qgroup_by_id(const BtrfsQgroup::id_t& id);

	/**
	 * @copydoc find_btrfs_qgroup_by_id(const BtrfsQgroup::id_t&)
	 */
	const BtrfsQgroup* find_btrfs_qgroup_by_id(const BtrfsQgroup::id_t& id) const;

	/**
	 * Get all Btrfses.
	 */
	static std::vector<Btrfs*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Btrfs*> get_all(const Devicegraph* devicegraph);

	bool get_configure_snapper() const ST_DEPRECATED;
	void set_configure_snapper(bool configure) ST_DEPRECATED;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Btrfs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Btrfs(Impl* impl);
	ST_NO_SWIG Btrfs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Btrfs.
     *
     * @throw NullPointerException
     */
    bool is_btrfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Btrfs.
     *
     * @return Pointer to Btrfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Btrfs* to_btrfs(Device* device);

    /**
     * @copydoc to_btrfs(Device*)
     */
    const Btrfs* to_btrfs(const Device* device);

}

#endif
