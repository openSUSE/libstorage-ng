/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Action.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"


namespace storage
{

    namespace Action
    {

	string
	Base::details() const
	{
	    string ret = "sid:" + to_string(sid);

	    if (first)
		ret += ", first";

	    if (last)
		ret += ", last";

	    if (only_sync)
		ret += ", only-sync";

	    if (nop)
		ret += ", nop";

	    return ret;
	}


	Text
	Create::text(const CommitData& commit_data) const
	{
	    return get_device(commit_data.actiongraph)->get_impl().do_create_text(commit_data.tense);
	}


	void
	Create::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    Device* device = get_device(commit_data.actiongraph);

	    device->get_impl().do_create();
	    device->get_impl().do_create_post_verify();
	}


	Text
	Delete::text(const CommitData& commit_data) const
	{
	    return get_device(commit_data.actiongraph)->get_impl().do_delete_text(commit_data.tense);
	}


	void
	Delete::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    get_device(commit_data.actiongraph)->get_impl().do_delete();
	}


	void
	Create::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
				 Actiongraph::Impl& actiongraph) const
	{
	    Base::add_dependencies(vertex, actiongraph);

	    sid_t sid = actiongraph[vertex]->sid;

	    const Device* device = actiongraph.find_device(sid, RHS);

	    for (const Device* parent : device->get_parents(View::ALL))
	    {
		sid_t parent_sid = parent->get_sid();

		if (!actiongraph.get_devicegraph(LHS)->device_exists(parent_sid))
		{
		    // parents must be created before child if not already existing

		    Actiongraph::Impl::vertex_descriptor tmp = actiongraph.actions_with_sid(parent_sid, ONLY_LAST).front();
		    actiongraph.add_edge(tmp, vertex);
		}
		else if (!get_device(actiongraph)->get_impl().has_dependency_manager())
		{
		    // all children must be deleted before parents

		    const Device* parent = actiongraph.find_device(parent_sid, LHS);

		    for (const Device* child : parent->get_children(View::REMOVE))
		    {
			sid_t child_sid = child->get_sid();

			vector<Actiongraph::Impl::vertex_descriptor> tmp = actiongraph.actions_with_sid(child_sid, ONLY_LAST);
			if (!tmp.empty())
			{
			    // Make sure it is a delete action
			    const Action::Base* tmp_action = actiongraph[tmp.front()];
			    if (is_delete(tmp_action))
				actiongraph.add_edge(tmp.front(), vertex);
			}
		    }
		}
	    }
	}


	void
	Delete::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
				 Actiongraph::Impl& actiongraph) const
	{
	    Base::add_dependencies(vertex, actiongraph);

	    // all children must be deleted before parents

	    sid_t sid = actiongraph[vertex]->sid;

	    const Device* device = actiongraph.find_device(sid, LHS);

	    for (const Device* parent : device->get_parents(View::REMOVE))
	    {
		sid_t parent_sid = parent->get_sid();

		for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.actions_with_sid(parent_sid, ONLY_FIRST))
		    actiongraph.add_edge(vertex, tmp);
	    }
	}


	const BlkDevice*
	RenameIn::get_renamed_blk_device(const Actiongraph::Impl& actiongraph, Side side) const
	{
	    return to_blk_device(actiongraph.get_devicegraph(side)->find_device(blk_device->get_sid()));
	}

    }

}
