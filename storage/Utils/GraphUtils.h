/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_GRAPH_UTILS_H
#define STORAGE_GRAPH_UTILS_H


#include <vector>
#include <map>


namespace storage
{
    using std::vector;
    using std::map;


    class CycleDetector : public boost::default_dfs_visitor
    {
    public:

	CycleDetector(bool& has_cycle) : has_cycle(has_cycle) {}

	template <class Edge, class Graph>
	void back_edge(Edge, const Graph&) { has_cycle = true; }

    protected:

	bool& has_cycle;

    };


    template <typename Vertex>
    class VertexRecorder : public boost::default_bfs_visitor
    {
    public:

	VertexRecorder(bool only_leaves, vector<Vertex>& vertices)
	    : only_leaves(only_leaves), vertices(vertices) {}

	template<typename Graph>
	void discover_vertex(Vertex v, const Graph& g) const
	{
	    if (!only_leaves || out_degree(v, g) == 0)
		vertices.push_back(v);
	}

    protected:

	const bool only_leaves;

	vector<Vertex>& vertices;

    };


    /*
     * Generates a vertex index (0 <= index < number of vertices) map.
     *
     * With VertexList=listS the adjacency_list does not automatically have a
     * vertex_index property.  Since some algorithm we use need that property
     * we have to create it ourself.  See:
     * http://www.boost.org/doc/libs/1_56_0/libs/graph/doc/faq.html
     */
    template <typename Graph>
    class VertexIndexMapGenerator
    {
    public:

	typedef map<typename Graph::vertex_descriptor, typename Graph::vertices_size_type> vertex_index_map_t;

	VertexIndexMapGenerator(const Graph& graph)
	    : graph(graph), vertex_index_property_map(vertex_index_map)
	{
	    typename Graph::vertices_size_type cnt = 0;

	    typename Graph::vertex_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = boost::vertices(graph); vi != vi_end; ++vi)
		boost::put(vertex_index_property_map, *vi, cnt++);
	}

	const boost::associative_property_map<vertex_index_map_t>& get() const
	{
	    return vertex_index_property_map;
	}

    private:

	const Graph& graph;

	vertex_index_map_t vertex_index_map;

	boost::associative_property_map<vertex_index_map_t> vertex_index_property_map;

    };

}

#endif
