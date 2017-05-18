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


#include "storage/ActiongraphImpl.h"
#include "storage/Action.h"


namespace storage
{

    Actiongraph::Actiongraph(const Storage& storage, const Devicegraph* lhs, Devicegraph* rhs)
	: impl(new Impl(storage, lhs, rhs))
    {
    }


    Actiongraph::~Actiongraph()
    {
    }


    const Storage&
    Actiongraph::get_storage() const
    {
	return get_impl().get_storage();
    }


    const Devicegraph*
    Actiongraph::get_devicegraph(Side side) const
    {
	return get_impl().get_devicegraph(side);
    }


    bool
    Actiongraph::empty() const
    {
	return get_impl().empty();
    }


    size_t
    Actiongraph::num_actions() const
    {
	return get_impl().num_actions();
    }


    vector<const Action::Base*>
    Actiongraph::get_commit_actions() const
    {
	return get_impl().get_commit_actions();
    }


    vector<string>
    Actiongraph::get_commit_actions_as_strings() const
    {
	const CommitData commit_data(get_impl(), Tense::SIMPLE_PRESENT);

	vector<string> ret;
	for (const Action::Base* action : get_commit_actions())
	    ret.push_back(action->text(commit_data).translated);

	return ret;
    }


    void
    Actiongraph::set_compound_actions()
    {
	get_impl().set_compound_actions(this);
    }


    std::vector<CompoundAction*>
    Actiongraph::get_compound_actions() const
    {
	return get_impl().get_compound_actions();
    }


    void
    Actiongraph::print_graph() const
    {
	get_impl().print_graph();
    }


    void
    Actiongraph::write_graphviz(const string& filename, GraphvizFlags graphviz_flags) const
    {
	get_impl().write_graphviz(filename, graphviz_flags);
    }


    void
    Actiongraph::print_order() const
    {
	get_impl().print_order();
    }

}
