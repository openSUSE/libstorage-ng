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


#ifndef STORAGE_COMPOUND_ACTION_CREATOR_H
#define STORAGE_COMPOUND_ACTION_CREATOR_H


#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>

#include "storage/CompoundAction.h"


namespace storage
{

    namespace Action
    {
	class Base;
    }

    class Actiongraph; 

    class CompoundActionCreator : private boost::noncopyable
    {

    public:

	CompoundActionCreator(const Actiongraph* actiongraph);
	~CompoundActionCreator();

	std::vector<const Action::Base*> get_commit_actions() const;

	std::vector<CompoundAction*> get_compound_actions() const;

	void group_commit_actions();

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:
    
	const std::unique_ptr<Impl> impl;
    
    };

}

#endif

