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


#include "storage/Actions/Reallot.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Reallot::text(const CommitData& commit_data) const
	{
	    const Device* device_lhs = get_device(commit_data.actiongraph, LHS);
	    return device_lhs->get_impl().do_reallot_text(commit_data, this);
	}


	void
	Reallot::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Device* device_rhs = get_device(commit_data.actiongraph, RHS);
	    device_rhs->get_impl().do_reallot(commit_data, this);
	}


	uf_t
	Reallot::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const Device* device = get_device(actiongraph, RHS);
	    return device->get_impl().do_reallot_used_features();
	}


	void
	Reallot::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
				  Actiongraph::Impl& actiongraph) const
	{
	    Modify::add_dependencies(vertex, actiongraph);

	    if (reallot_mode == ReallotMode::REDUCE)
	    {
		// Make sure the device (PV) is removed before being destroyed
		for (Actiongraph::Impl::vertex_descriptor tmp :
			 actiongraph.actions_with_sid(device->get_sid(), ONLY_FIRST))
		    actiongraph.add_edge(vertex, tmp);
	    }
	    else
	    {
		// Make sure the device is created before being added
		for (Actiongraph::Impl::vertex_descriptor tmp :
			 actiongraph.actions_with_sid(device->get_sid(), ONLY_LAST))
		    actiongraph.add_edge(tmp, vertex);

		// If the device was assigned elsewhere, make sure it's removed
		// from there before being re-assigned
		for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.vertices())
		    if (action_removes_device(actiongraph[tmp]))
		    {
			actiongraph.add_edge(tmp, vertex);
			break;
		    }
	    }
	}


	bool
	Reallot::action_removes_device(const Action::Base* action) const
	{
	    const Action::Reallot* reallot = dynamic_cast<const Action::Reallot*>(action);

	    if (!reallot)
		return false;

	    if (reallot->reallot_mode != ReallotMode::REDUCE)
		return false;

	    return reallot->device->get_sid() == device->get_sid();
	}

    }

}
