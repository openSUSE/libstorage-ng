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


#ifndef STORAGE_COMPOUND_ACTION_IMPL_H
#define STORAGE_COMPOUND_ACTION_IMPL_H


#include "storage/CompoundAction.h"


namespace storage
{
    
    using std::vector;
    using std::string;

    class CompoundAction::Impl
    {
    public:

	Impl(const Actiongraph* actiongraph);
	~Impl();

	void set_target_device(const Device* device);

	const Device* get_target_device() const;

	void set_commit_actions(vector<const Action::Base*> actions);

	vector<const Action::Base*> get_commit_actions() const; 

	void add_commit_action(const Action::Base* action);
    
	vector<string> get_commit_actions_as_strings() const;

	string to_string() const;

    private:
	
	const Actiongraph* actiongraph;

	const Device* target_device;

	vector<const Action::Base*> commit_actions;
    
    };

}

#endif

