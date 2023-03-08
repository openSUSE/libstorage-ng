/*
 * Copyright (c) [2017-2023] SUSE LLC
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


#include "storage/Actions/RemoveFromEtcFstabImpl.h"
#include "storage/Filesystems/MountPointImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	RemoveFromEtcFstab::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, LHS));
	    return mount_point->get_impl().do_remove_from_etc_fstab_text(commit_data.tense);
	}


	void
	RemoveFromEtcFstab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, LHS));
	    mount_point->get_impl().do_remove_from_etc_fstab(commit_data);
	}

    }

}
