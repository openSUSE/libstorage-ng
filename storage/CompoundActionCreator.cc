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


#include "storage/CompoundActionCreatorImpl.h"


namespace storage
{

    using std::vector;


    CompoundActionCreator::CompoundActionCreator(const Actiongraph* actiongraph)
    : impl(new Impl(actiongraph)) {}


    CompoundActionCreator::~CompoundActionCreator() {}
    

    vector<const Action::Base*>
    CompoundActionCreator::get_commit_actions() const
    {
	return get_impl().get_commit_actions();
    }


    vector<CompoundAction*>
    CompoundActionCreator::get_compound_actions() const
    {
	return get_impl().get_compound_actions();
    }


    void
    CompoundActionCreator::group_commit_actions()
    {
	return get_impl().group_commit_actions();
    }

}
