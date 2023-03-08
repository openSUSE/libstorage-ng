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


#include "storage/Actions/DeleteImpl.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Delete::text(const CommitData& commit_data) const
	{
	    switch (affect)
	    {
		case Affect::DEVICE:
		    return get_device(commit_data.actiongraph)->get_impl().do_delete_text(commit_data.tense);

		case Affect::HOLDER:
		    return get_holder(commit_data.actiongraph)->get_impl().do_delete_text(commit_data.tense);
	    }

	    ST_THROW(LogicException("unknown Action::Affect"));
	}


	void
	Delete::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    switch (affect)
	    {
		case Affect::DEVICE:
		    get_device(commit_data.actiongraph)->get_impl().do_delete();
		    break;

		case Affect::HOLDER:
		    get_holder(commit_data.actiongraph)->get_impl().do_delete();
		    break;
	    }
	}


	uf_t
	Delete::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    switch (affect)
	    {
		case Affect::DEVICE:
		{
		    const Device* device = get_device(actiongraph);
		    return device->get_impl().do_delete_used_features();
		}

		case Affect::HOLDER:
		{
		    const Holder* holder = get_holder(actiongraph);
		    return holder->get_impl().do_delete_used_features();
		}
	    }

	    ST_THROW(LogicException("unknown Action::Affect"));
	}


	Device*
	Delete::get_device(const Actiongraph::Impl& actiongraph) const
	{
	    if (!affects_device())
		ST_THROW(Exception("requested device for action not affecting device, " + details()));

	    return actiongraph.get_devicegraph(LHS)->find_device(sid);
	}


	Holder*
	Delete::get_holder(const Actiongraph::Impl& actiongraph) const
	{
	    if (!affects_holder())
		ST_THROW(Exception("requested holder for action not affecting holder, " + details()));

	    return actiongraph.get_devicegraph(LHS)->find_holder(sid_pair.first, sid_pair.second);
	}


	void
	Delete::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex, Actiongraph::Impl& actiongraph) const
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
	Delete::add_device_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
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


	void
	Delete::add_holder_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
	    get_holder(actiongraph)->get_impl().add_dependencies(vertex, actiongraph);
	}

    }

}
