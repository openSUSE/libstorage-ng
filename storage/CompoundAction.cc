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


#include "storage/CompoundActionImpl.h"


namespace storage
{

    CompoundAction::CompoundAction(const Actiongraph* actiongraph)
    : impl(new Impl(actiongraph)) {}


    CompoundAction::CompoundAction(const Actiongraph* actiongraph, const Action::Base* action)
    : impl(new Impl(actiongraph, action)) {}


    CompoundAction::~CompoundAction() {}


    void
    CompoundAction::set_target_device(const Device* device)
    {
	get_impl().set_target_device(device);	
    }


    const Device*
    CompoundAction::get_target_device() const
    {
	return get_impl().get_target_device();
    }


    void
    CompoundAction::set_commit_actions(vector<const Action::Base*> actions)
    {
	get_impl().set_commit_actions(actions);
    }


    vector<const Action::Base*>
    CompoundAction::get_commit_actions() const
    {
	return get_impl().get_commit_actions();
    }


    void
    CompoundAction::add_commit_action(const Action::Base* action)
    {
	get_impl().add_commit_action(action);
    }


    string
    CompoundAction::string_representation() const
    {
	return get_impl().string_representation();
    }


    bool
    CompoundAction::is_delete() const
    {
	return get_impl().is_delete();
    }


    vector<CompoundAction*> 
    CompoundAction::generate(const Actiongraph* actiongraph)
    {
	return CompoundAction::Impl::generate(actiongraph);    
    }

}

