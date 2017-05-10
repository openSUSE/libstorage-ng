/*
 * Copyright (c) 2017 SUSE LLC
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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include <boost/algorithm/string/join.hpp>

#include "storage/CompoundActionImpl.h"
#include "storage/ActiongraphImpl.h"


namespace storage
{

    CompoundAction::Impl::Impl(const Actiongraph* actiongraph)
    : actiongraph(actiongraph), target_device(nullptr) 
    {}


    CompoundAction::Impl::~Impl() {}


    void CompoundAction::Impl::set_target_device(const Device* device)
    {
	this->target_device = device;
    }


    const Device* CompoundAction::Impl::get_target_device() const
    {
	return target_device;
    }

    void CompoundAction::Impl::set_commit_actions(vector<const Action::Base*> actions)
    {
	this->commit_actions = actions;
    }


    vector<const Action::Base*> CompoundAction::Impl::get_commit_actions() const
    {
	return commit_actions;
    }


    void CompoundAction::Impl::add_commit_action(const Action::Base* action)
    {
	commit_actions.push_back(action);
    }


    vector<string>
    CompoundAction::Impl::get_commit_actions_as_strings() const
    {
	const CommitData commit_data(actiongraph->get_impl(), Tense::SIMPLE_PRESENT);

	vector<string> ret;
	for (const Action::Base* action : commit_actions)
	    ret.push_back(action->text(commit_data).translated);

	return ret;
    }


    //TODO Generate text depending on the set of commit actions.
    string
    CompoundAction::Impl::to_string() const
    {
	return boost::algorithm::join(get_commit_actions_as_strings(), " and ");	
    }

}

