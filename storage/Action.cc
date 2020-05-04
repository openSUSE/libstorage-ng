/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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

	    Devicegraph::Impl::vertex_descriptor v_in_rhs = actiongraph.get_devicegraph(RHS)->get_impl().find_vertex(sid);

	    // iterate parents
	    Devicegraph::Impl::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_rhs, actiongraph.get_devicegraph(RHS)->get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t parent_sid = actiongraph.get_devicegraph(RHS)->get_impl()[*vi]->get_sid();

		if (!actiongraph.get_devicegraph(LHS)->device_exists(parent_sid))
		{
		    // parents must be created beforehand if not existed

		    Actiongraph::Impl::vertex_descriptor tmp = actiongraph.actions_with_sid(parent_sid, ONLY_LAST).front();
		    actiongraph.add_edge(tmp, vertex);
		}
		else
		{
		    // children of parents must be deleted beforehand

		    if (!get_device(actiongraph)->get_impl().has_dependency_manager())
		    {
			Devicegraph::Impl::vertex_descriptor q = actiongraph.get_devicegraph(LHS)->get_impl().find_vertex(parent_sid);

			Devicegraph::Impl::adjacency_iterator vi2, vi2_end;
			for (boost::tie(vi2, vi2_end) = adjacent_vertices(q, actiongraph.get_devicegraph(LHS)->get_impl().graph); vi2 != vi2_end; ++vi2)
			{
			    sid_t child_sid = actiongraph.get_devicegraph(LHS)->get_impl()[*vi2]->get_sid();

			    vector<Actiongraph::Impl::vertex_descriptor> tmp = actiongraph.actions_with_sid(child_sid, ONLY_LAST);
			    if (!tmp.empty())
			    {
				// Make sure it's a delete action
				const Action::Base* tmp_action = actiongraph[tmp.front()];
				if (is_delete(tmp_action))
				    actiongraph.add_edge(tmp.front(), vertex);
			    }
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

	    // all children must be deleted beforehand

	    sid_t sid = actiongraph[vertex]->sid;

	    Devicegraph::Impl::vertex_descriptor v_in_lhs = actiongraph.get_devicegraph(LHS)->get_impl().find_vertex(sid);

	    // iterate children
	    Devicegraph::Impl::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_lhs, actiongraph.get_devicegraph(LHS)->get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t child_sid = actiongraph.get_devicegraph(RHS)->get_impl()[*vi]->get_sid();

		for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.actions_with_sid(child_sid, ONLY_FIRST))
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
