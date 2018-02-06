/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#ifndef STORAGE_ACTIONGRAPH_H
#define STORAGE_ACTIONGRAPH_H


#include <memory>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>

#include "storage/Graphviz.h"
#include "storage/CompoundAction.h"


namespace storage
{

    class Storage;
    class Devicegraph;


    namespace Action
    {
	class Base;
    }


    enum Side {
	LHS, RHS
    };


    class Actiongraph : private boost::noncopyable
    {
    public:

	/**
	 * @throw Exception
	 */
	Actiongraph(const Storage& storage, const Devicegraph* lhs, Devicegraph* rhs);

	~Actiongraph();

	const Storage& get_storage() const;

	const Devicegraph* get_devicegraph(Side side) const;

	void print_graph() const;

	/**
	 * @throw Exception
	 */
	void write_graphviz(const std::string& filename, GraphvizFlags flags = GraphvizFlags::NAME,
			    GraphvizFlags tooltip_flags = GraphvizFlags::NONE) const;

	void print_order() const;

	bool empty() const;

	size_t num_actions() const;

	/**
	 * Sorted according to dependencies among actions.
	 */
	std::vector<const Action::Base*> get_commit_actions() const;

	// TODO add Action to the public interface and use get_commit_actions instead
	std::vector<std::string> get_commit_actions_as_strings() const;

	void generate_compound_actions();
	std::vector<const CompoundAction*> get_compound_actions() const;

    public:

        class Impl;

        Impl& get_impl() { return *impl; }
        const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}


#endif
