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


#include "storage/Actions/RepairImpl.h"
#include "storage/Devices/GptImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Repair::text(const CommitData& commit_data) const
	{
	    const Gpt* gpt = to_gpt(get_device(commit_data.actiongraph, RHS));
	    return gpt->get_impl().do_repair_text(commit_data.tense);
	}


	void
	Repair::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Gpt* gpt = to_gpt(get_device(commit_data.actiongraph, RHS));
	    gpt->get_impl().do_repair();
	}

    }

}
