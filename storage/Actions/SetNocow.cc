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


#include "storage/Actions/SetNocow.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	SetNocow::text(const CommitData& commit_data) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_nocow_text(commit_data.tense);
	}


	void
	SetNocow::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(commit_data.actiongraph, RHS));
	    btrfs_subvolume->get_impl().do_set_nocow();
	}


	uf_t
	SetNocow::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const BtrfsSubvolume* btrfs_subvolume = to_btrfs_subvolume(get_device(actiongraph, RHS));
	    return btrfs_subvolume->get_impl().do_set_nocow_used_features();
	}

    }

}
