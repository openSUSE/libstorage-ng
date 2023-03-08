/*
 * Copyright (c) [2016-2020] SUSE LLC
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


#include "storage/Actions/AttachBcacheCsetImpl.h"
#include "storage/Devices/BcacheImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	AttachBcacheCset::text(const CommitData& commit_data) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    return bcache->get_impl().do_attach_bcache_cset_text(commit_data.tense);
	}


	void
	AttachBcacheCset::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Bcache* bcache = to_bcache(get_device(commit_data.actiongraph, RHS));
	    bcache->get_impl().do_attach_bcache_cset();
	}

    }

}
