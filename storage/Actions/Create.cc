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


#include "storage/Actions/Create.h"
#include "storage/Actions/Delete.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Create::text(const CommitData& commit_data) const
	{
	    switch (affect)
	    {
		case Affect::DEVICE:
		    return get_device(commit_data.actiongraph)->get_impl().do_create_text(commit_data.tense);

		case Affect::HOLDER:
		    return get_holder(commit_data.actiongraph)->get_impl().do_create_text(commit_data.tense);
	    }

	    ST_THROW(LogicException("unknown Action::Affect"));
	}


	void
	Create::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    switch (affect)
	    {
		case Affect::DEVICE:
		{
		    Device* device = get_device(commit_data.actiongraph);

		    device->get_impl().do_create();
		    device->get_impl().do_create_post_verify();
		}
		break;

		case Affect::HOLDER:
		{
		    Holder* holder = get_holder(commit_data.actiongraph);

		    holder->get_impl().do_create();
		}
		break;
	    }
	}


	uf_t
	Create::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    switch (affect)
	    {
		case Affect::DEVICE:
		{
		    const Device* device = get_device(actiongraph);
		    return device->get_impl().do_create_used_features();
		}

		case Affect::HOLDER:
		{
		    const Holder* holder = get_holder(actiongraph);
		    return holder->get_impl().do_create_used_features();
		}
	    }

	    ST_THROW(LogicException("unknown Action::Affect"));
	}


	Device*
	Create::get_device(const Actiongraph::Impl& actiongraph) const
	{
	    if (!affects_device())
		ST_THROW(Exception("requested device for action not affecting device, " + details()));

	    return actiongraph.get_devicegraph(RHS)->find_device(sid);
	}


	Holder*
	Create::get_holder(const Actiongraph::Impl& actiongraph) const
	{
	    if (!affects_holder())
		ST_THROW(Exception("requested holder for action not affecting holder, " + details()));

	    return actiongraph.get_devicegraph(RHS)->find_holder(sid_pair.first, sid_pair.second);
	}


	void
	Create::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
				 Actiongraph::Impl& actiongraph) const
	{
	    switch (affect)
	    {
		case Affect::DEVICE:
		    add_device_dependencies(vertex, actiongraph);
		    break;

		case Affect::HOLDER:
		    add_holder_dependencies(vertex, actiongraph);
		    break;
	    }
	}


	void
	Create::add_device_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
	    sid_t sid = actiongraph[vertex]->sid;

	    const Device* device = actiongraph.find_device(sid, RHS);

	    for (const Device* parent : device->get_parents(View::ALL))
	    {
		sid_t parent_sid = parent->get_sid();

		if (!actiongraph.get_devicegraph(LHS)->device_exists(parent_sid))
		{
		    // parents must be created before child if not already existing
		    vector<Actiongraph::Impl::vertex_descriptor> tmp = actiongraph.actions_with_sid(parent_sid, ONLY_LAST);
		    if (tmp.empty())
		    {
			// there might be create sync-only actions missing
			ST_THROW(Exception("no parent create action"));
		    }

		    actiongraph.add_edge(tmp.front(), vertex);
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
	Create::add_holder_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
	    get_holder(actiongraph)->get_impl().add_dependencies(vertex, actiongraph);
	}

    }

}
