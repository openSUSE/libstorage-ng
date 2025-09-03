/*
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_BTRFS_QGROUP_H
#define STORAGE_BTRFS_QGROUP_H


#include "storage/Devicegraph.h"
#include "storage/Devices/Device.h"


namespace storage
{

    class Btrfs;


    /**
     * Class to represent a qgroup of a btrfs filesystem.
     *
     * Not included in the classic view.
     *
     * @see Btrfs
     */
    class BtrfsQgroup : public Device
    {
    public:

	/**
	 * Type for a btrfs qgroup id. The first value is the level, the second one the
	 * id.
	 *
	 * btrfs will only use 16 bits of the level and 48 bits of the id.
	 */
	using id_t = std::pair<unsigned int, unsigned long long>;

	/**
	 * @see Device::create(Devicegraph*)
	 */
	static BtrfsQgroup* create(Devicegraph* devicegraph, const id_t& id);

	static BtrfsQgroup* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the btrfs filesystem this qgroup belongs to.
	 */
	Btrfs* get_btrfs();

	/**
	 * @copydoc get_btrfs()
	 */
	const Btrfs* get_btrfs() const;

	/**
	 * Get the id of the qgroup.
	 */
	id_t get_id() const;

	/**
	 * Get the referenced size of the qgroup.
	 */
	unsigned long long get_referenced() const;

	/**
	 * Get the exclusive size of the qgroup.
	 */
	unsigned long long get_exclusive() const;

	/**
	 * Return whether the qgroup has a limit for the referenced size.
	 */
	bool has_referenced_limit() const;

	/**
	 * Return the limit for the referenced size of the qgroup.
	 *
	 * @throw Exception
	 */
	unsigned long long get_referenced_limit() const;

	/**
	 * Set the limit for the referenced size of the qgroup.
	 */
	void set_referenced_limit(unsigned long long referenced_limit);

	/**
	 * Clear the limit for the referenced size of the qgroup.
	 */
	void clear_referenced_limit();

	/**
	 * Return whether the qgroup has a limit for the exclusive size.
	 */
	bool has_exclusive_limit() const;

	/**
	 * Return the limit for the exclusive size of the qgroup.
	 *
	 * @throw Exception
	 */
	unsigned long long get_exclusive_limit() const;

	/**
	 * Set the limit for the exclusive size of the qgroup.
	 */
	void set_exclusive_limit(unsigned long long exclusive_limit);

	/**
	 * Clear the limit for the exclusive size of the qgroup.
	 */
	void clear_exclusive_limit();

	/**
	 * Return whether the btrfs_qgroup is assigned to the qgroup.
	 */
	bool is_assigned(const BtrfsQgroup* btrfs_qgroup) const;

	/**
	 * Assign btrfs_qgroup to the qgroup.
	 *
	 * @throw Exception
	 */
	void assign(BtrfsQgroup* btrfs_qgroup);

	/**
	 * Unassign btrfs_qgroup from the qgroup.
	 *
	 * @throw Exception
	 */
	void unassign(BtrfsQgroup* btrfs_qgroup);

	/**
	 * Get all qgroups assigned to the qgroup.
	 */
	std::vector<BtrfsQgroup*> get_assigned_btrfs_qgroups();

	/**
	 * @copydoc get_assigned_btrfs_qgroups()
	 */
	std::vector<const BtrfsQgroup*> get_assigned_btrfs_qgroups() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual BtrfsQgroup* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	BtrfsQgroup(Impl* impl);
	ST_NO_SWIG BtrfsQgroup(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Btrfs.
     *
     * @throw NullPointerException
     */
    bool is_btrfs_qgroup(const Device* device);

    /**
     * Converts pointer to Device to pointer to Btrfs.
     *
     * @return Pointer to Btrfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    BtrfsQgroup* to_btrfs_qgroup(Device* device);

    /**
     * @copydoc to_btrfs(Device*)
     */
    const BtrfsQgroup* to_btrfs_qgroup(const Device* device);

}

#endif
