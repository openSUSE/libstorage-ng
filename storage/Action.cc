/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
#include "storage/Devices/Partition.h"


namespace storage
{

    namespace Action
    {

	Text
	Create::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    return get_device_rhs(actiongraph)->get_impl().do_create_text(tense);
	}


	void
	Create::commit(const Actiongraph::Impl& actiongraph) const
	{
	    get_device_rhs(actiongraph)->get_impl().do_create();
	}


	Text
	Delete::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    return get_device_lhs(actiongraph)->get_impl().do_delete_text(tense);
	}


	void
	Delete::commit(const Actiongraph::Impl& actiongraph) const
	{
	    get_device_lhs(actiongraph)->get_impl().do_delete();
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
		    if (!is_partition(get_device_rhs(actiongraph)))
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
				if (dynamic_cast<const Action::Delete*>(tmp_action))
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

	    if (!is_partition(get_device_lhs(actiongraph)))
	    {
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
	}

    }

}
