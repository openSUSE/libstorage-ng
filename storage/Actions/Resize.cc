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


#include "storage/Actions/Resize.h"
#include "storage/Actions/Create.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Resize::text(const CommitData& commit_data) const
	{
	    const Device* device = get_device(commit_data.actiongraph, get_side());
	    return device->get_impl().do_resize_text(commit_data, this);
	}


	void
	Resize::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Device* device = get_device(commit_data.actiongraph, get_side());
	    device->get_impl().do_resize(commit_data, this);
	}


	uf_t
	Resize::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const Device* device = get_device(actiongraph, get_side());
	    return device->get_impl().do_resize_used_features();
	}


	void
	Resize::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
				 Actiongraph::Impl& actiongraph) const
	{
	    Modify::add_dependencies(vertex, actiongraph);

	    if (!actiongraph[vertex]->affects_device())
		return;

	    // The disabled dependencies are already created in
	    // Action.cc. TODO Should be more consistent.

	    /*
	    const Devicegraph* devicegraph_lhs = actiongraph.get_devicegraph(LHS);
	    const Device* device_lhs = devicegraph_lhs->find_device(actiongraph[vertex]->sid);
	    */

	    const Devicegraph* devicegraph_rhs = actiongraph.get_devicegraph(RHS);
	    const Device* device_rhs = devicegraph_rhs->find_device(actiongraph[vertex]->sid);

	    for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.vertices())
	    {
		Action::Base* action = actiongraph[tmp];

		if (!action->affects_device())
		    continue;

		if (is_create(action) && is_child(device_rhs, action->sid))
		    actiongraph.add_edge(vertex, tmp);

		/*
		if (is_delete(action) && is_child(device_lhs, action->sid))
		    actiongraph.add_edge(tmp, vertex);
		*/
	    }
	}


	const BlkDevice*
	Resize::get_resized_blk_device(const Actiongraph::Impl& actiongraph, Side side) const
	{
	    return to_blk_device(actiongraph.get_devicegraph(side)->find_device(blk_device->get_sid()));
	}


	bool
	Resize::is_child(const Device* device, sid_t sid) const
	{
	    vector<const Device*> children = device->get_impl().get_children_of_type<const Device>();

	    return any_of(children.begin(), children.end(), [sid](const Device* child) {
		return child->get_sid() == sid;
	    });
	}

    }

}
