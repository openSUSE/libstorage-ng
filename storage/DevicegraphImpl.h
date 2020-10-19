/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_DEVICEGRAPH_IMPL_H
#define STORAGE_DEVICEGRAPH_IMPL_H


#include <set>
#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>

#include "storage/Devices/Device.h"
#include "storage/Holders/Holder.h"
#include "storage/Devicegraph.h"
#include "storage/View.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::set;
    using std::pair;


    class Devicegraph::Impl : private boost::noncopyable
    {

    public:

	// Using OutEdgeList=boost::listS allows parallel edges.  Using
	// VertexList=boost::listS and OutEdgeList=boost::listS makes both
	// vertex and edge iterators stable (never invalidated unless deleted). See:
	// http://www.boost.org/doc/libs/1_56_0/libs/graph/doc/adjacency_list.html

	// The shared_ptr is required for runtime polymorphism of Device and
	// Holder.  Using newer bundled properties instead of cumbersome internal
	// properties, see:
	// http://www.boost.org/doc/libs/1_56_0/libs/graph/doc/bundles.html

	typedef boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS,
				      std::shared_ptr<Device>, std::shared_ptr<Holder>> graph_t;

	typedef graph_t::vertex_descriptor vertex_descriptor;
	typedef graph_t::edge_descriptor edge_descriptor;

	typedef graph_t::vertex_iterator vertex_iterator;
	typedef graph_t::edge_iterator edge_iterator;

	typedef graph_t::adjacency_iterator adjacency_iterator;
	typedef graph_t::inv_adjacency_iterator inv_adjacency_iterator;

	typedef graph_t::in_edge_iterator in_edge_iterator;
	typedef graph_t::out_edge_iterator out_edge_iterator;

	typedef graph_t::vertices_size_type vertices_size_type;

	typedef std::function<bool(Devicegraph::Impl::edge_descriptor)> edge_filter_t;

	typedef boost::filtered_graph<graph_t, edge_filter_t> filtered_graph_t;


	Impl(Storage* storage) : storage(storage) {}

	bool operator==(const Impl& rhs) const;
	bool operator!=(const Impl& rhs) const { return !(*this == rhs); }

	void check(const CheckCallbacks* check_callbacks) const;

	uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const;

	void log_diff(std::ostream& log, const Impl& rhs) const;

	/**
	 * Check if this devicegraph is the probed devicegraph.
	 */
	bool is_probed() const;

	/**
	 * Check if this devicegraph is the staging devicegraph.
	 */
	bool is_staging() const;

	/**
	 * Check if this devicegraph is the system devicegraph.
	 */
	bool is_system() const;

	bool empty() const;

	size_t num_devices() const;
	size_t num_holders() const;

	set<sid_t> get_device_sids() const;
	set<pair<sid_t, sid_t>> get_holder_sids() const;

	vertex_descriptor add_vertex(Device* device);
	edge_descriptor add_edge(vertex_descriptor source_vertex, vertex_descriptor target_vertex,
				 Holder* holder);

	bool device_exists(sid_t sid) const;
	bool holder_exists(sid_t source_sid, sid_t target_sid) const;

	vertex_descriptor find_vertex(sid_t sid) const;
	edge_descriptor find_edge(sid_t source_sid, sid_t target_sid) const;
	vector<edge_descriptor> find_edges(sid_t source_sid, sid_t target_sid) const;

	vertex_descriptor source(edge_descriptor edge) const { return boost::source(edge, graph); }
	vertex_descriptor target(edge_descriptor edge) const { return boost::target(edge, graph); }

	edge_descriptor set_source(edge_descriptor edge, vertex_descriptor vertex);

	Device* operator[](vertex_descriptor vertex) { return graph[vertex].get(); }
	const Device* operator[](vertex_descriptor vertex) const { return graph[vertex].get(); }

	Holder* operator[](edge_descriptor edge) { return graph[edge].get(); }
	const Holder* operator[](edge_descriptor edge) const { return graph[edge].get(); }

	void clear();

	void remove_vertex(vertex_descriptor vertex);
	void remove_edge(edge_descriptor edge);

	boost::iterator_range<vertex_iterator> vertices() const;
	boost::iterator_range<edge_iterator> edges() const;

	void load(Devicegraph* devicegraph, const string& filename);
	void save(const string& filename) const;

	void print(std::ostream& out) const;

	void write_graphviz(const string& filename, DevicegraphStyleCallbacks* style_callbacks,
			    View view = View::CLASSIC) const;

	size_t num_children(vertex_descriptor vertex, View view = View::CLASSIC) const;
	size_t num_parents(vertex_descriptor vertex, View view = View::CLASSIC) const;

	vertex_descriptor child(vertex_descriptor vertex, View view = View::CLASSIC) const;
	vertex_descriptor parent(vertex_descriptor vertex, View view = View::CLASSIC) const;

	// TODO use iterator base functions instead of vector based below
	// boost::iterator_range<adjacency_iterator> children(vertex_descriptor vertex) const;
	// boost::iterator_range<inv_adjacency_iterator> parents(vertex_descriptor vertex) const;

	vector<vertex_descriptor> children(vertex_descriptor vertex, View view = View::CLASSIC) const;
	vector<vertex_descriptor> parents(vertex_descriptor vertex, View view = View::CLASSIC) const;
	vector<vertex_descriptor> siblings(vertex_descriptor vertex, bool itself, View view = View::CLASSIC) const;
	vector<vertex_descriptor> descendants(vertex_descriptor vertex, bool itself, View view = View::CLASSIC) const;
	vector<vertex_descriptor> ancestors(vertex_descriptor vertex, bool itself, View view = View::CLASSIC) const;
	vector<vertex_descriptor> leaves(vertex_descriptor vertex, bool itself, View view = View::CLASSIC) const;
	vector<vertex_descriptor> roots(vertex_descriptor vertex, bool itself, View view = View::CLASSIC) const;

	edge_descriptor in_edge(vertex_descriptor vertex, View view = View::CLASSIC) const;
	edge_descriptor out_edge(vertex_descriptor vertex, View view = View::CLASSIC) const;

	vector<edge_descriptor> in_edges(vertex_descriptor vertex, View view = View::CLASSIC) const;
	vector<edge_descriptor> out_edges(vertex_descriptor vertex, View view = View::CLASSIC) const;


	template<typename Type>
	vector<Type*>
	get_devices_of_type() const
	{
	    vector<Type*> ret;

	    for (vertex_descriptor vertex : vertices())
	    {
		Type* device = dynamic_cast<Type*>(graph[vertex].get());
		if (device)
		    ret.push_back(device);
	    }

	    return ret;
	}


	template <typename Type, typename Pred>
	vector<Type*>
	get_devices_of_type_if(Pred pred) const
	{
	    vector<Type*> ret;

	    for (vertex_descriptor vertex : vertices())
	    {
		Type* device = dynamic_cast<Type*>(graph[vertex].get());
		if (device && pred(device))
		    ret.push_back(device);
	    }

	    return ret;
	}


	template <typename Type>
	vector<Type*>
	filter_devices_of_type(const vector<vertex_descriptor>& vertices)
	{
	    vector<Type*> ret;

	    for (vertex_descriptor vertex : vertices)
	    {
		Type* device = dynamic_cast<Type*>(graph[vertex].get());
		if (device)
		    ret.push_back(device);
	    }

	    return ret;
	}


	template <typename Type>
	vector<const Type*>
	filter_devices_of_type(const vector<vertex_descriptor>& vertices) const
	{
	    vector<const Type*> ret;

	    for (vertex_descriptor vertex : vertices)
	    {
		const Type* device = dynamic_cast<const Type*>(graph[vertex].get());
		if (device)
		    ret.push_back(device);
	    }

	    return ret;
	}


	template <typename Type>
	vector<Type*>
	filter_holders_of_type(const vector<edge_descriptor>& edges)
	{
	    vector<Type*> ret;

	    for (edge_descriptor edge : edges)
	    {
		Type* holder = dynamic_cast<Type*>(graph[edge].get());
		if (holder)
		    ret.push_back(holder);
	    }

	    return ret;
	}


	template <typename Type>
	vector<const Type*>
	filter_holders_of_type(const vector<edge_descriptor>& edges) const
	{
	    vector<const Type*> ret;

	    for (edge_descriptor edge : edges)
	    {
		const Type* holder = dynamic_cast<const Type*>(graph[edge].get());
		if (holder)
		    ret.push_back(holder);
	    }

	    return ret;
	}


	void swap(Devicegraph::Impl& x);

	Storage* get_storage() { return storage; }
	const Storage* get_storage() const { return storage; }

	graph_t graph;		// TODO private?

    private:

	edge_filter_t make_edge_filter(View view) const;

	Storage* storage;

    };

}

#endif
