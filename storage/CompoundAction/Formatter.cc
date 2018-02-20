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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include "storage/CompoundAction/Formatter.h"
#include "storage/Filesystems/MountPoint.h"


namespace storage
{

    using std::string;

    CompoundAction::Formatter::Formatter(const CompoundAction::Impl* compound_action)
    : compound_action(compound_action) {}


    CompoundAction::Formatter::~Formatter() {}


    string
    CompoundAction::Formatter::string_representation() const
    {
	return text().translated;
    }

    const BlkFilesystem*
    CompoundAction::Formatter::get_created_filesystem() const
    {
	auto action = get_create<storage::BlkFilesystem>();

	if (action)
	{
	    auto device = CompoundAction::Impl::device(compound_action->get_actiongraph(), action);
	    return to_blk_filesystem(device);
	}

	return nullptr;
    }


    const MountPoint*
    CompoundAction::Formatter::get_created_mount_point() const
    {
	auto action = get_create<storage::MountPoint>();

	if (action)
	{
	    auto device = CompoundAction::Impl::device(compound_action->get_actiongraph(), action);
	    return to_mount_point(device);
	}

	return nullptr;
    }


    Text
    CompoundAction::Formatter::default_text() const
    {
	const CommitData commit_data(compound_action->get_actiongraph()->get_impl(), Tense::SIMPLE_PRESENT);
	auto first_action = compound_action->get_commit_actions().front();
	return first_action->text(commit_data);
    }

}
