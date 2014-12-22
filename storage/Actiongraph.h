#ifndef ACTIONGRAPH_H
#define ACTIONGRAPH_H


#include <list>
#include <map>
#include <deque>
#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::list;
    using std::map;
    using std::deque;


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

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
				      std::shared_ptr<Action::Base>> graph_t;

	typedef graph_t::vertex_descriptor vertex_descriptor;
	typedef graph_t::edge_descriptor edge_descriptor;

	typedef graph_t::vertex_iterator vertex_iterator;
	typedef graph_t::edge_iterator edge_iterator;

	typedef graph_t::vertices_size_type vertices_size_type;

	Actiongraph(const Devicegraph* lhs, const Devicegraph* rhs);

	const Devicegraph* get_devicegraph(Side side) const { return side == LHS ? lhs : rhs; }

	vertex_descriptor add_vertex(Action::Base* action);

	edge_descriptor add_edge(vertex_descriptor a, vertex_descriptor b);

	void add_chain(vector<Action::Base*>& actions);

	vector<vertex_descriptor> huhu(sid_t sid, bool first, bool last) const; // TODO better concept

	boost::iterator_range<vertex_iterator> vertices() const;

	void print_graph() const;
	void write_graphviz(const std::string& filename) const;

	graph_t graph;

	list<string> get_commit_steps() const;
	void commit() const;

	// TODO simple_t is useful for comparing in testsuite, move there?
	typedef map<string, vector<string>> simple_t;
	simple_t get_simple() const;

    private:

	void get_actions();
	void add_dependencies();
	void get_order();

	const Devicegraph* lhs;
	const Devicegraph* rhs;

	typedef deque<vertex_descriptor> Order;

	Order order;

    };

}


namespace std
{

    ostream& operator<<(ostream& s, const storage::Actiongraph::simple_t& simple);

}


#endif
