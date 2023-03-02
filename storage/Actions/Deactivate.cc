/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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


#include "storage/Actions/Deactivate.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Deactivate::text(const CommitData& commit_data) const
	{
	    const Device* device = get_device(commit_data.actiongraph, LHS);
	    return device->get_impl().do_deactivate_text(commit_data.tense);
	}


	void
	Deactivate::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Device* device = get_device(commit_data.actiongraph, LHS);
	    device->get_impl().do_deactivate();
	}


	uf_t
	Deactivate::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const Device* device = get_device(actiongraph, LHS);
	    return device->get_impl().do_deactivate_used_features();
	}

    }
}
