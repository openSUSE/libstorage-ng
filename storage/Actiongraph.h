#ifndef ACTION_GRAPH_H
#define ACTION_GRAPH_H


#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"


namespace storage_bgl
{

    namespace Action
    {
	class Base;
    }


    class Actiongraph : private boost::noncopyable
    {
    public:

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
				      shared_ptr<Action::Base>> graph_t;

	typedef graph_t::vertex_descriptor vertex_descriptor;
	typedef graph_t::edge_descriptor edge_descriptor;

	typedef graph_t::vertex_iterator vertex_iterator;
	typedef graph_t::edge_iterator edge_iterator;

	typedef graph_t::vertices_size_type vertices_size_type;

	Actiongraph(const Devicegraph& lhs, const Devicegraph& rhs);

	vertex_descriptor add_vertex(Action::Base* action);

	edge_descriptor add_edge(vertex_descriptor a, vertex_descriptor b);

	void add_chain(vector<Action::Base*>& actions);

	vector<vertex_descriptor> huhu(sid_t sid, bool first, bool last) const; // TODO better concept

	boost::iterator_range<vertex_iterator> vertices() const;

	void print_graph() const;
	void write_graphviz(const string& filename) const;

	const Devicegraph& lhs;
	const Devicegraph& rhs;

	graph_t graph;

	void commit() const;

	// TODO simple_t is useful for comparing in testsuite, move there?
	typedef map<string, vector<string>> simple_t;
	simple_t simple() const;

    private:

	void get_actions();
	void add_dependencies();
	void reduce();
	void get_order();

	typedef deque<vertex_descriptor> Order;

	Order order;

    };

}


namespace std
{

    ostream& operator<<(ostream& s, const storage_bgl::Actiongraph::simple_t& simple);

}


#endif
