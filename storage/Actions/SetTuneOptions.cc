/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Actions/SetTuneOptions.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	SetTuneOptions::text(const CommitData& commit_data) const
	{
	    const BlkFilesystem* blk_filesystem = to_blk_filesystem(get_device(commit_data.actiongraph, RHS));
	    return blk_filesystem->get_impl().do_set_tune_options_text(commit_data.tense);
	}


	void
	SetTuneOptions::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const BlkFilesystem* blk_filesystem = to_blk_filesystem(get_device(commit_data.actiongraph, RHS));
	    blk_filesystem->get_impl().do_set_tune_options();
	}


	uf_t
	SetTuneOptions::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const BlkFilesystem* blk_filesystem = to_blk_filesystem(get_device(actiongraph, RHS));
	    return blk_filesystem->get_impl().do_set_tune_options_used_features();
	}

    }

}
