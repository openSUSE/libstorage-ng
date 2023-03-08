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


#include "storage/Actions/AddToEtcMdadmImpl.h"
#include "storage/Devices/MdImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	AddToEtcMdadm::text(const CommitData& commit_data) const
	{
	    const Md* md = to_md(get_device(commit_data.actiongraph, RHS));
	    return md->get_impl().do_add_to_etc_mdadm_text(commit_data.tense);
	}


	void
	AddToEtcMdadm::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Md* md = to_md(get_device(commit_data.actiongraph, RHS));
	    md->get_impl().do_add_to_etc_mdadm(commit_data);
	}


	void
	AddToEtcMdadm::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
	    Modify::add_dependencies(vertex, actiongraph);

	    if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		actiongraph.add_edge(*actiongraph.mount_root_filesystem, vertex);
	}

    }

}
