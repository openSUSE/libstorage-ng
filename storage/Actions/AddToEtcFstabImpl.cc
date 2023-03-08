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


#include "storage/Actions/AddToEtcFstabImpl.h"
#include "storage/Filesystems/MountPointImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	AddToEtcFstab::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, RHS));
	    return mount_point->get_impl().do_add_to_etc_fstab_text(commit_data.tense);
	}


	void
	AddToEtcFstab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, RHS));
	    mount_point->get_impl().do_add_to_etc_fstab(commit_data);
	}


	FsType
	AddToEtcFstab::get_fs_type(const Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph, RHS));
	    return mount_point->get_mount_type();
	}


	const string&
	AddToEtcFstab::get_path(const Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph, RHS));
	    return mount_point->get_path();
	}


	void
	AddToEtcFstab::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
	    // For non swap the dependency from mount-of-root to add-to-fstab is redundant.

	    if (get_fs_type(actiongraph) == FsType::SWAP)
		if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		    actiongraph.add_edge(*actiongraph.mount_root_filesystem, vertex);
	}

    }

}
