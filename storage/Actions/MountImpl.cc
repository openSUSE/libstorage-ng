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


#include "storage/Actions/MountImpl.h"
#include "storage/Filesystems/MountPointImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Mount::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    return mount_point->get_impl().do_mount_text(commit_data.tense);
	}


	void
	Mount::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    mount_point->get_impl().do_mount(commit_data, commit_options);
	}


	FsType
	Mount::get_fs_type(const Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph));
	    return mount_point->get_mount_type();
	}


	const string&
	Mount::get_path(const Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph));
	    return mount_point->get_path();
	}


	string
	Mount::get_rootprefixed_path(const Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph));
	    return mount_point->get_impl().get_rootprefixed_path();
	}


	const MountPoint*
	Mount::get_mount_point(const Actiongraph::Impl& actiongraph) const
	{
	    return to_mount_point(get_device(actiongraph));
	}


	uf_t
	Mount::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph));
	    return mount_point->get_impl().do_mount_used_features();
	}

    }

}
