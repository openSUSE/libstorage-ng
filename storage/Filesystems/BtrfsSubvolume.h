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


#ifndef STORAGE_BTRFS_SUBVOLUME_H
#define STORAGE_BTRFS_SUBVOLUME_H


#include "storage/Filesystems/Mountable.h"
#include "storage/Devicegraph.h"


namespace storage
{

    class Btrfs;


    /**
     * Class to represent a btrfs subvolume.
     */
    class BtrfsSubvolume : public Mountable
    {
    public:

	static BtrfsSubvolume* create(Devicegraph* devicegraph, const std::string& path);
	static BtrfsSubvolume* load(Devicegraph* devicegraph, const xmlNode* node);

	long get_id() const;

	bool is_top_level() const;

	const std::string& get_path() const;

	bool is_nocow() const;
	void set_nocow(bool nocow);

	bool is_default_btrfs_subvolume() const;
	void set_default_btrfs_subvolume();

	Btrfs* get_btrfs();
	const Btrfs* get_btrfs() const;

	BtrfsSubvolume* get_top_level_btrfs_subvolume();
	const BtrfsSubvolume* get_top_level_btrfs_subvolume() const;

	BtrfsSubvolume* create_btrfs_subvolume(const std::string& path);

	/**
	 * Compare (less than) two BtrfsSubvolumes by id.
	 */
	static bool compare_by_id(const BtrfsSubvolume* lhs, const BtrfsSubvolume* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual BtrfsSubvolume* clone() const override;

    protected:

	BtrfsSubvolume(Impl* impl);

    };


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
