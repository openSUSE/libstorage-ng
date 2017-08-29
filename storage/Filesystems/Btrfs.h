/*
 * Copyright (c) 2015 Novell, Inc.
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


#ifndef STORAGE_BTRFS_H
#define STORAGE_BTRFS_H


#include "storage/Devicegraph.h"
#include "storage/Filesystems/BlkFilesystem.h"


namespace storage
{

    class BtrfsSubvolume;


    class BtrfsSubvolumeNotFoundByPath : public DeviceNotFound
    {
    public:

	BtrfsSubvolumeNotFoundByPath(const std::string& path);
    };


    class Btrfs : public BlkFilesystem
    {
    public:

	static Btrfs* create(Devicegraph* devicegraph);
	static Btrfs* load(Devicegraph* devicegraph, const xmlNode* node);

	BtrfsSubvolume* get_top_level_btrfs_subvolume();
	const BtrfsSubvolume* get_top_level_btrfs_subvolume() const;

	BtrfsSubvolume* get_default_btrfs_subvolume();
	const BtrfsSubvolume* get_default_btrfs_subvolume() const;

	void set_default_btrfs_subvolume(BtrfsSubvolume* btrfs_subvolume) const;

	std::vector<BtrfsSubvolume*> get_btrfs_subvolumes();
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

        bool get_configure_snapper() const;
        void set_configure_snapper(bool configure);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Btrfs* clone() const override;

    protected:

	Btrfs(Impl* impl);

    };


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
