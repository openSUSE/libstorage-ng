/*
 * Copyright (c) [2016-2022] SUSE LLC
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


#ifndef STORAGE_USED_FEATURES_H
#define STORAGE_USED_FEATURES_H


#include <stdint.h>
#include <string>


namespace storage
{

    /**
     * Enum specifying the dependency type of used features.
     */
    enum class UsedFeaturesDependencyType
    {
	/**
	 * Required used features only include the minimal set of used features.
	 */
	REQUIRED,

	/**
	 * Suggested used features include compared to the required used features
	 * also filesystems that have no mount point.
	 */
	SUGGESTED
    };


    /**
     * Convert the UsedFeaturesDependencyType used_features_dependency_type to a string.
     *
     * @see UsedFeaturesDependencyType
     */
    std::string get_used_features_dependency_type_name(UsedFeaturesDependencyType used_features_dependency_type);


    /**
     * Type for used features.
     */
    using uf_t = uint64_t;


    enum : uint64_t		// TODO use uf_t
    {
	UF_EXT2 = 1 << 0,
	UF_EXT3 = 1 << 1,
	UF_EXT4 = 1 << 2,
	UF_BTRFS = 1 << 3,
	UF_XFS = 1 << 4,
	UF_REISERFS = 1 << 5,
	UF_SWAP = 1 << 6,
	UF_NTFS = 1 << 7,
	UF_VFAT = 1 << 8,
	UF_NFS = 1 << 9,
	UF_JFS = 1 << 10,
	UF_F2FS = 1 << 23,
	UF_NILFS2 = 1 << 30,
	UF_EXFAT = 1 << 24,
	UF_UDF = 1 << 25,
	UF_BITLOCKER = 1 << 27,

	UF_PLAIN_ENCRYPTION = 1 << 26,
	UF_LUKS = 1 << 11,

	UF_LVM = 1 << 12,
	UF_MDRAID = 1 << 13,
	UF_DMRAID = 1 << 14,
	UF_MULTIPATH = 1 << 15,
	UF_BCACHE = 1 << 16,

	UF_ISCSI = 1 << 17,
	UF_FCOE = 1 << 18,
	UF_FC = 1 << 19,
	UF_DASD = 1 << 20,
	UF_PMEM = 1 << 29,
	UF_NVME = 1 << 28,

	/** Quota for ext4 or xfs. Not for btrfs. */
	UF_QUOTA = 1 << 21,

	UF_SNAPSHOTS = 1 << 22
    };


    /**
     * Return a string with the names of the used features. Not for production code.
     */
    std::string get_used_features_names(uf_t used_features);

}

#endif
