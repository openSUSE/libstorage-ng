/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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


#include "storage/Actions/SetLabel.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	SetLabel::text(const CommitData& commit_data) const
	{
	    const Device* device = get_device(commit_data.actiongraph, RHS);

	    if (is_partition(device))
	    {
		const Partition* partition = to_partition(device);
		return partition->get_impl().do_set_label_text(commit_data.tense);
	    }

	    if (is_blk_filesystem(device))
	    {
		const BlkFilesystem* blk_filesystem = to_blk_filesystem(device);
		return blk_filesystem->get_impl().do_set_label_text(commit_data.tense);
	    }

	    ST_THROW(LogicException("SetLabel called for unsupported object"));
	}


	void
	SetLabel::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Device* device = get_device(commit_data.actiongraph, RHS);

	    if (is_partition(device))
	    {
		const Partition* partition = to_partition(device);
		return partition->get_impl().do_set_label();
	    }

	    if (is_blk_filesystem(device))
	    {
		const BlkFilesystem* blk_filesystem = to_blk_filesystem(device);
		return blk_filesystem->get_impl().do_set_label();
	    }

	    ST_THROW(LogicException("SetLabel called for unsupported object"));
	}


	uf_t
	SetLabel::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const Device* device = get_device(actiongraph, RHS);

	    if (is_partition(device))
	    {
		const Partition* partition = to_partition(device);
		return partition->get_impl().do_set_label_used_features();
	    }

	    if (is_blk_filesystem(device))
	    {
		const BlkFilesystem* blk_filesystem = to_blk_filesystem(device);
		return blk_filesystem->get_impl().do_set_label_used_features();
	    }

	    ST_THROW(LogicException("SetLabel called for unsupported object"));
	}

    }

}
