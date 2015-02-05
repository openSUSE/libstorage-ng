#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H


#include <string>
#include <vector>
#include <map>


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


    class cycle_detector : public boost::default_dfs_visitor
    {
    public:

	cycle_detector(bool& has_cycle) : has_cycle(has_cycle) {}

	template <class Edge, class Graph>
	void back_edge(Edge, const Graph&) { has_cycle = true; }

    protected:

	bool& has_cycle;

    };


    // TODO rename, to unspecific

    template <typename Vertex>
    class vertex_recorder : public boost::default_bfs_visitor
    {
    public:

	vertex_recorder(bool only_leafs, vector<Vertex>& vertices)
	    : only_leafs(only_leafs), vertices(vertices) {}

	template<typename Graph>
	void discover_vertex(Vertex v, const Graph& g) const
	{
	    if (!only_leafs || out_degree(v, g) == 0)
		vertices.push_back(v);
	}

    protected:

	bool only_leafs;

	vector<Vertex>& vertices;

    };


    // With VertexList=listS the adjacency_list does not automatically have a
    // vertex_index property.  Since some algorithm we use need that property
    // we have to create it ourself.  See:
    // http://www.boost.org/doc/libs/1_56_0/libs/graph/doc/faq.html

    // TODO rename

    template <typename Graph>
    class Haha
    {
    public:

	typedef map<typename Graph::vertex_descriptor, typename Graph::vertices_size_type> vertex_index_map_t;

	Haha(const Graph& graph)
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
