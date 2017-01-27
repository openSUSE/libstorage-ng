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


#include <boost/graph/copy.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

#include "config.h"
#include "storage/Utils/Stopwatch.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Action.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    Actiongraph::Impl::Impl(const Storage& storage, const Devicegraph* lhs, const Devicegraph* rhs)
	: storage(storage), lhs(lhs), rhs(rhs)
    {
	lhs->check();
	rhs->check();

	Stopwatch stopwatch;

	get_actions();
	set_special_actions();
	add_dependencies();
	remove_only_syncs();

	calculate_order();

	y2mil("stopwatch " << stopwatch << " for actiongraph generation");
    }


    bool
    Actiongraph::Impl::empty() const
    {
	return boost::num_vertices(graph) == 0;
    }


    size_t
    Actiongraph::Impl::num_actions() const
    {
	return boost::num_vertices(graph);
    }


    Actiongraph::Impl::vertex_descriptor
    Actiongraph::Impl::add_vertex(Action::Base* action)
    {
	return boost::add_vertex(shared_ptr<Action::Base>(action), graph);
    }


    Actiongraph::Impl::edge_descriptor
    Actiongraph::Impl::add_edge(vertex_descriptor a, vertex_descriptor b)
    {
	pair<edge_descriptor, bool> tmp = boost::add_edge(a, b, graph);

	return tmp.first;
    }


    void
    Actiongraph::Impl::add_chain(const vector<Action::Base*>& actions)
    {
	vertex_descriptor v1 = 0;

	for (vector<Action::Base*>::const_iterator it = actions.begin(); it != actions.end(); ++it)
	{
	    (*it)->first = it == actions.begin();
	    (*it)->last = it == --actions.end();

	    vertex_descriptor v2 = add_vertex(*it);

	    if (it != actions.begin())
		add_edge(v1, v2);

	    v1 = v2;
	}
    }


    void
    Actiongraph::Impl::add_chain(const vector<vertex_descriptor>& actions)
    {
	if (actions.size() > 1)
	{
	    Actiongraph::Impl::vertex_descriptor vertex = actions[0];
	    for (size_t i = 1; i < actions.size(); ++i)
	    {
		add_edge(vertex, actions[i]);
		vertex = actions[i];
	    }
	}
    }


    void
    Actiongraph::Impl::add_chain(const vector<vector<vertex_descriptor>>& vert_vectors)
    {
	vector<vector<Actiongraph::Impl::vertex_descriptor>> non_empty_vectors;

	for (const vector<vertex_descriptor>& vector : vert_vectors)
	    if (!vector.empty())
		non_empty_vectors.push_back(vector);

	if (non_empty_vectors.size() > 1)
	    for (size_t i = 1; i < non_empty_vectors.size(); ++i)
		// As an alternative implementation, we could add sync actions
		// here instead of creating all the edges right away
		for (vertex_descriptor left : non_empty_vectors[i - 1])
		    for (vertex_descriptor right : non_empty_vectors[i])
			add_edge(left, right);
    }


    const vector<Actiongraph::Impl::vertex_descriptor>&
    Actiongraph::Impl::actions_with_sid(sid_t sid) const
    {
	map<sid_t, vector<vertex_descriptor>>::const_iterator it = cache_for_actions_with_sid.find(sid);
	if (it != cache_for_actions_with_sid.end())
	    return it->second;

	const static vector<vertex_descriptor> empty;
	return empty;
    }


    vector<Actiongraph::Impl::vertex_descriptor>
    Actiongraph::Impl::actions_with_sid(sid_t sid, ActionsFilter actions_filter) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor vertex : actions_with_sid(sid))
	{
	    const Action::Base* action = graph[vertex].get();

	    if (actions_filter == ONLY_FIRST && !action->first)
		continue;

	    if (actions_filter == ONLY_LAST && !action->last)
		continue;

	    ret.push_back(vertex);
	}

	return ret;
    }


    boost::iterator_range<Actiongraph::Impl::vertex_iterator>
    Actiongraph::Impl::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    boost::iterator_range<Actiongraph::Impl::adjacency_iterator>
    Actiongraph::Impl::children(vertex_descriptor vertex) const
    {
	return boost::make_iterator_range(boost::adjacent_vertices(vertex, graph));
    }


    boost::iterator_range<Actiongraph::Impl::inv_adjacency_iterator>
    Actiongraph::Impl::parents(vertex_descriptor vertex) const
    {
	return boost::make_iterator_range(boost::inv_adjacent_vertices(vertex, graph));
    }


    void
    Actiongraph::Impl::get_actions()
    {
	const set<sid_t> lhs_sids = lhs->get_impl().get_device_sids();
	const set<sid_t> rhs_sids = rhs->get_impl().get_device_sids();

	vector<sid_t> created_sids;
	set_difference(rhs_sids.begin(), rhs_sids.end(), lhs_sids.begin(), lhs_sids.end(),
		       back_inserter(created_sids));

	vector<sid_t> common_sids;
	set_intersection(lhs_sids.begin(), lhs_sids.end(), rhs_sids.begin(), rhs_sids.end(),
			 back_inserter(common_sids));

	vector<sid_t> deleted_sids;
	set_difference(lhs_sids.begin(), lhs_sids.end(), rhs_sids.begin(), rhs_sids.end(),
		       back_inserter(deleted_sids));

	for (sid_t sid : created_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_rhs = rhs->get_impl().find_vertex(sid);
	    const Device* d_rhs = rhs->get_impl()[v_rhs];

	    d_rhs->get_impl().add_create_actions(*this);
	}

	for (sid_t sid : common_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    const Device* d_lhs = lhs->get_impl()[v_lhs];

	    Devicegraph::Impl::vertex_descriptor v_rhs = rhs->get_impl().find_vertex(sid);
	    const Device* d_rhs = rhs->get_impl()[v_rhs];

	    d_rhs->get_impl().add_modify_actions(*this, d_lhs);
	}

	for (sid_t sid : deleted_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    const Device* d_lhs = lhs->get_impl()[v_lhs];

	    d_lhs->get_impl().add_delete_actions(*this);
	}
    }


    void
    Actiongraph::Impl::set_special_actions()
    {
	boost::iterator_range<vertex_iterator> range = vertices();

	mount_root_filesystem = range.end();

	for (vertex_iterator it = range.begin(); it != range.end(); ++it)
	{
	    const Action::Base* action = graph[*it].get();

	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(action);
	    if (mount && mount->mountpoint == "/")
		mount_root_filesystem = it;

	    cache_for_actions_with_sid[action->sid].push_back(*it);
	}
    }


    void
    Actiongraph::Impl::add_dependencies()
    {
	vector<vertex_descriptor> mounts;

	// TODO also for Devices only in LHS?

	const Devicegraph* devicegraph = get_devicegraph(RHS);

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const Device* device = devicegraph->get_impl()[vertex];

	    device->get_impl().add_dependencies(*this);
	}

	PartitionTable::Impl::add_all_dependencies(*this);

	for (vertex_descriptor vertex : vertices())
	{
	    graph[vertex]->add_dependencies(vertex, *this);

	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(graph[vertex].get());
	    if (mount && mount->mountpoint != "swap")
		mounts.push_back(vertex);
	}

	if (mounts.size() > 1)
	{
	    // TODO correct sort
	    sort(mounts.begin(), mounts.end(), [this, &mounts](vertex_descriptor l, vertex_descriptor r) {
		const Action::Mount* ml = dynamic_cast<const Action::Mount*>(graph[l].get());
		const Action::Mount* mr = dynamic_cast<const Action::Mount*>(graph[r].get());
		return ml->mountpoint <= mr->mountpoint;
	    });

	    add_chain(mounts);
	}
    }


    void
    Actiongraph::Impl::remove_only_syncs()
    {
	vector<vertex_descriptor> only_syncs;

	for (vertex_descriptor vertex : vertices())
	{
	    if ((*this)[vertex]->only_sync)
		only_syncs.push_back(vertex);
	}

	for (vertex_descriptor vertex : only_syncs)
	{
	    for (vertex_descriptor parent : parents(vertex))
		for (vertex_descriptor child : children(vertex))
		    add_edge(parent, child);

	    clear_vertex(vertex, graph);
	    remove_vertex(vertex, graph);
	}
    }


    void
    Actiongraph::Impl::calculate_order()
    {
	VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	try
	{
	    boost::topological_sort(graph, front_inserter(order),
				    vertex_index_map(vertex_index_map_generator.get()));
	}
	catch (const boost::not_a_dag&)
	{
	    cerr << "action graph not a DAG" << endl;
	}
    }


    void
    Actiongraph::Impl::print_order() const
    {
	for (const vertex_descriptor& vertex : order)
	{
	   const Action::Base* action = graph[vertex].get();
	   cout << action->text(*this, Tense::SIMPLE_PRESENT).native << endl;
	}

	cout << endl;
    }


    vector<const Action::Base*>
    Actiongraph::Impl::get_commit_actions() const
    {
	vector<const Action::Base*> commit_actions;

	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();

	    commit_actions.push_back(action);
	}

	return commit_actions;
    }


    void
    Actiongraph::Impl::commit(const CommitCallbacks* commit_callbacks) const
    {
	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();

	    string text = action->text(*this, Tense::PRESENT_CONTINUOUS).translated;

	    y2mil("Commit Action \"" << text << "\"");

	    if (commit_callbacks)
	    {
		commit_callbacks->message(text);
		commit_callbacks->pre(action);
	    }

	    try
	    {
		action->commit(*this);
	    }
	    catch (const Exception& e)
	    {
		ST_CAUGHT(e);

		if (!commit_callbacks || !commit_callbacks->error(text, e.what()))
		    ST_RETHROW(e);

		y2mil("user decides to continue after error");
	    }

	    if (commit_callbacks)
	    {
		commit_callbacks->post(action);
	    }
	}
    }


    void
    Actiongraph::Impl::print_graph() const
    {
	typedef map<vertex_descriptor, string> vertex_name_map_t;
	vertex_name_map_t vertex_name_map;
	boost::associative_property_map<vertex_name_map_t> my_vertex_name_map(vertex_name_map);

	for (vertex_descriptor v : vertices())
	{
	    string text = "[ " + graph[v]->text(*this, Tense::SIMPLE_PRESENT).translated + " ]";
	    boost::put(my_vertex_name_map, v, text);
	}

	boost::print_graph(graph, my_vertex_name_map);

	cout << endl;
    }


    namespace
    {

	struct write_graph
	{
	    write_graph(const Actiongraph::Impl&) {}

	    void operator()(ostream& out) const
	    {
		out << "node [ shape=rectangle, style=filled, fontname=\"Arial\" ];" << endl;
	    }
	};


	struct write_vertex
	{
	    write_vertex(const Actiongraph::Impl& actiongraph, GraphvizFlags graphviz_flags)
		: actiongraph(actiongraph), graphviz_flags(graphviz_flags) {}

	    const Actiongraph::Impl& actiongraph;
	    const GraphvizFlags graphviz_flags;

	    void operator()(ostream& out, const Actiongraph::Impl::vertex_descriptor& vertex) const
	    {
		const Action::Base* action = actiongraph[vertex];

		string label = action->text(actiongraph, Tense::SIMPLE_PRESENT).translated;
		string tooltip = action->text(actiongraph, Tense::SIMPLE_PRESENT).translated;

		string& extra = (graphviz_flags && GraphvizFlags::TOOLTIP) ? tooltip : label;

		if (graphviz_flags && GraphvizFlags::SID)
		{
		    extra += "\\n" "sid:" + to_string(action->sid);

		    extra += " [";
		    if (action->first)
			extra += "f";
		    if (action->last)
			extra += "l";
		    if (action->only_sync)
			extra += "s";
		    extra += "]";
		}

		out << "[ label=" << boost::escape_dot_string(label);

		if (is_create(action))
		    out << ", color=\"#00ff00\", fillcolor=\"#ccffcc\"";
		else if (is_modify(action))
		    out << ", color=\"#0000ff\", fillcolor=\"#ccccff\"";
		else if (is_delete(action))
		    out << ", color=\"#ff0000\", fillcolor=\"#ffcccc\"";
		else
		    ST_THROW(LogicException("unknown Action::Base subclass"));

		if (graphviz_flags && GraphvizFlags::TOOLTIP)
		{
		    out << ", tooltip=" << boost::escape_dot_string(tooltip);
		}

		out << " ]";
	    }
	};

    }


    void
    Actiongraph::Impl::write_graphviz(const string& filename, GraphvizFlags graphviz_flags) const
    {
	ofstream fout(filename);

	fout << "// generated by libstorage-ng version " VERSION << endl;
	fout << endl;

	VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	boost::write_graphviz(fout, graph, write_vertex(*this, graphviz_flags),
			      boost::default_writer(), write_graph(*this),
			      vertex_index_map_generator.get());

	fout.close();
    }

}
