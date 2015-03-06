#ifndef ACTIONGRAPH_H
#define ACTIONGRAPH_H


#include <list>
#include <map>
#include <deque>
#include <set>
#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::list;
    using std::map;
    using std::deque;
    using std::set;


    class Storage;
    class CommitCallbacks;


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

	Actiongraph(const Storage& storage, const Devicegraph* lhs, const Devicegraph* rhs);

	const Storage& get_storage() const { return storage; }

	const Devicegraph* get_devicegraph(Side side) const { return side == LHS ? lhs : rhs; }

	vertex_descriptor add_vertex(Action::Base* action);

	edge_descriptor add_edge(vertex_descriptor a, vertex_descriptor b);

	void add_chain(vector<Action::Base*>& actions);

	vector<vertex_descriptor> huhu(sid_t sid, bool first, bool last) const; // TODO better concept

	boost::iterator_range<vertex_iterator> vertices() const;

	Text get_action_text(vertex_descriptor v, bool doing) const;

	void print_graph() const;
	void write_graphviz(const string& filename, bool details = false) const;

	graph_t graph;

	list<string> get_commit_steps() const;
	void commit(const CommitCallbacks* commit_callbacks) const;

	// special actions
	vertex_iterator mount_root_filesystem;

    private:

	void get_actions();
	void add_dependencies();
	void set_special_actions();
	void get_order();

	const Storage& storage;

	const Devicegraph* lhs;
	const Devicegraph* rhs;

	typedef deque<vertex_descriptor> Order;

	Order order;

    };


    class Cmp			// TODO rename, maybe move to testsuite
    {
    public:

	typedef vector<string> expected_t;

	Cmp(const Actiongraph& actiongraph, const expected_t& expected);

	bool ok() const { return errors.empty(); }

	friend std::ostream& operator<<(std::ostream& out, const Cmp& cmp);

    private:

	struct Entry
	{
	    Entry(const string& line);

	    string id;
	    string text;
	    set<string> dep_ids;
	};

	vector<Entry> entries;

	void check() const;

	void cmp_texts(const Actiongraph& actiongraph);
	void cmp_dependencies(const Actiongraph& actiongraph);

	vector<string> errors;

    };

}

#endif
