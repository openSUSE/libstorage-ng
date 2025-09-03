/*
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


#ifndef STORAGE_BTRFS_SUBVOLUME_H
#define STORAGE_BTRFS_SUBVOLUME_H


#include "storage/Filesystems/Mountable.h"
#include "storage/Devicegraph.h"


namespace storage
{

    class Btrfs;
    class BtrfsQgroup;


    /**
     * Class to represent a btrfs subvolume in the devicegraph.
     *
     * @see Btrfs
     */
    class BtrfsSubvolume : public Mountable
    {
    public:

	/**
	 * Create a device of type BtrfsSubvolume. Usually this function is not called
	 * directly. Instead BtrfsSubvolume::create_btrfs_subvolume() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static BtrfsSubvolume* create(Devicegraph* devicegraph, const std::string& path);

	static BtrfsSubvolume* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the id of the btrfs subvolume. For newly created btrfs subvolumes the id is
	 * -1.
	 */
	long get_id() const;

	/**
	 * Check whether the subvolume is the top-level subvolume.
	 */
	bool is_top_level() const;

	/**
	 * Get the path of the btrfs subvolume.
	 */
	const std::string& get_path() const;

	/**
	 * Get the copy-on-write (COW) flag of the btrfs subvolume.
	 */
	bool is_nocow() const;

	/**
	 * Set the copy-on-write (COW) flag of the btrfs subvolume.
	 *
	 * @see is_nocow()
	 */
	void set_nocow(bool nocow);

	/**
	 * Check whether the subvolume is the default subvolume.
	 */
	bool is_default_btrfs_subvolume() const;

	/**
	 * Set the subvolume to be the default subvolume.
	 *
	 * @see is_default_btrfs_subvolume()
	 */
	void set_default_btrfs_subvolume();

	/**
	 * Return the btrfs filesystem of the btrfs subvolume.
	 */
	Btrfs* get_btrfs();

	/**
	 * @copydoc get_btrfs()
	 */
	const Btrfs* get_btrfs() const;

	/**
	 * Return the top-level subvolume.
	 *
	 * @see is_top_level()
	 */
	BtrfsSubvolume* get_top_level_btrfs_subvolume();

	/**
	 * @copydoc get_top_level_btrfs_subvolume()
	 */
	const BtrfsSubvolume* get_top_level_btrfs_subvolume() const;

	/**
	 * Check whether the btrfs subvolume has snapshots.
	 */
	bool has_snapshots() const;

	/**
	 * Get snapshots of the btrfs subvolume.
	 *
	 * @see has_snapshots()
	 */
	std::vector<BtrfsSubvolume*> get_snapshots();

	/**
	 * @copydoc get_snapshots()
	 */
	std::vector<const BtrfsSubvolume*> get_snapshots() const;

	/**
	 * Check whether the btrfs subvolume has an origin. In other words, whether it is
	 * a snapshot. Note that here a snapshot stops being considered a snapshot if the
	 * origin has been removed.
	 *
	 * The term "origin" is not used with btrfs but with LVM. Unfortunately btrfs is
	 * not consistent with the names: With 'btrfs subvolume snapshot' the thing is
	 * called "source", with 'btrfs subvolume list' the thing is called "parent_uuid"
	 * (but not "parent_id"). Both "source" and "parent" are already used in
	 * libstorage-ng.
	 *
	 * @see has_snapshots()
	 */
	bool has_origin() const;

	/**
	 * Get the origin of the btrfs subvolume if it has one.
	 *
	 * @see has_origin()
	 *
	 * @throw Exception
	 */
	BtrfsSubvolume* get_origin();

	/**
	 * @copydoc get_origin()
	 */
	const BtrfsSubvolume* get_origin() const;

	/**
	 * Return whether the subvolume as a corresponding level 0 qgroup.
	 */
	bool has_btrfs_qgroup() const;

	/**
	 * Return the corresponding level 0 qgroup of the subvolume.
	 *
	 * @throw Exception
	 */
	BtrfsQgroup* get_btrfs_qgroup();

	/**
	 * @copydoc get_btrfs_qgroup()
	 */
	const BtrfsQgroup* get_btrfs_qgroup() const;

	/**
	 * Create the corresponding level 0 qgroup for the subvolume. Usually the qgroup
	 * already exists (if quota is enabled) unless it was removed by the user. Quota
	 * must be enabled.
	 *
	 * @throw Exception
	 */
	BtrfsQgroup* create_btrfs_qgroup();

	/**
	 * Create a btrfs subvolume for the btrfs subvolume.
	 *
	 * If quota is enabled for the btrfs also the corresponding level 0 qgroup is
	 * created.
	 *
	 * @throw Exception
	 */
	BtrfsSubvolume* create_btrfs_subvolume(const std::string& path);

	/**
	 * Compare (less than) two BtrfsSubvolumes by id.
	 *
	 * @see get_id()
	 */
	static bool compare_by_id(const BtrfsSubvolume* lhs, const BtrfsSubvolume* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual BtrfsSubvolume* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	BtrfsSubvolume(Impl* impl);
	ST_NO_SWIG BtrfsSubvolume(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a BtrfsSubvolume.
     *
     * @throw NullPointerException
     */
    bool is_btrfs_subvolume(const Device* device);

    /**
     * Converts pointer to Device to pointer to BtrfsSubvolume.
     *
     * @return Pointer to BtrfsSubvolume.
     * @throw DeviceHasWrongType, NullPointerException
     */
    BtrfsSubvolume* to_btrfs_subvolume(Device* device);

    /**
     * @copydoc to_btrfs_subvolume(Device*)
     */
    const BtrfsSubvolume* to_btrfs_subvolume(const Device* device);

}

#endif
