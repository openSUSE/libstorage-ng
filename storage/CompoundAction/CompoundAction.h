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


#ifndef STORAGE_COMPOUND_ACTION_H
#define STORAGE_COMPOUND_ACTION_H


#include <memory>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>

#include "storage/Action.h"
#include "storage/Devices/Device.h"


namespace storage
{

    class Actiongraph;

    class CompoundAction : private boost::noncopyable
    {

    public:

	CompoundAction(const Actiongraph* actiongraph);
	CompoundAction(const Actiongraph* actiongraph, const Action::Base* action);
	~CompoundAction();

	void set_target_device(const Device* device);

	const Device* get_target_device() const;

	void set_commit_actions(std::vector<const Action::Base*> actions);

	std::vector<const Action::Base*> get_commit_actions() const; 

	void add_commit_action(const Action::Base* action);
	
	std::string string_representation() const;

	static std::vector<CompoundAction*> generate(const Actiongraph* actiongraph);

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:
    
	const std::unique_ptr<Impl> impl;
    
    };

}

#endif

