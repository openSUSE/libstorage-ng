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


#ifndef STORAGE_ACTION_UNMOUNT_IMPL_H
#define STORAGE_ACTION_UNMOUNT_IMPL_H


#include "storage/Actions/DeleteImpl.h"
#include "storage/Filesystems/Mountable.h"


namespace storage
{

    namespace Action
    {

	class Unmount : public Delete
	{
	public:

	    Unmount(sid_t sid) : Delete(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	    FsType get_fs_type(const Actiongraph::Impl& actiongraph) const;

	    const string& get_path(const Actiongraph::Impl& actiongraph) const;
	    string get_rootprefixed_path(const Actiongraph::Impl& actiongraph) const;

	};

    }


    inline bool
    is_unmount(const Action::Base* action)
    {
	return is_action_of_type<const Action::Unmount>(action);
    }

}

#endif
