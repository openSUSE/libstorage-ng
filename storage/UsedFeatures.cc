/*
 * Copyright (c) [2020-2022] SUSE LLC
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


#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "storage/UsedFeatures.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    template <> struct EnumTraits<UsedFeaturesDependencyType> { static const vector<string> names; };

    const vector<string> EnumTraits<UsedFeaturesDependencyType>::names({
	"REQUIRED", "SUGGESTED"
    });


    string
    get_used_features_dependency_type_name(UsedFeaturesDependencyType used_features_dependency_type)
    {
	return toString(used_features_dependency_type);
    }


    string
    get_used_features_names(uf_t used_features)
    {
	static const vector<string> names = { "ext2", "ext3", "ext4", "btrfs", "xfs",
					      "reiserfs", "swap", "ntfs", "vfat", "nfs",
					      "jfs", "luks", "lvm", "md-raid", "dm-raid",
					      "multipath", "bcache", "iscsi", "fcoe", "fc",
					      "dasd", "quota", "snapshot", "f2fs", "exfat",
					      "udf", "plain-encryption", "bitlocker", "nvme",
					      "pmem", "nilfs2", "bcachefs" };

	vector<string> tmp;

	for (unsigned int i = 0; i < names.size(); ++i)
	{
	    if (used_features & ((uf_t)(1) << i))
		tmp.push_back(names[i]);
	}

	sort(tmp.begin(), tmp.end());

	return boost::join(tmp, " ");
    }

}
