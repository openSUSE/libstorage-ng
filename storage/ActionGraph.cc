

#include <boost/graph/copy.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

#include "DeviceGraph.h"
#include "GraphUtils.h"
#include "Action.h"
#include "ActionGraph.h"


namespace storage
{

    ActionGraph::ActionGraph(const DeviceGraph& lhs, const DeviceGraph& rhs)
	: lhs(lhs), rhs(rhs)
    {
	cout << "lhs graph" << endl;
	lhs.print_graph();

	cout << "rhs graph" << endl;
	rhs.print_graph();

	get_actions();
	add_dependencies();

	cout << "action graph" << endl;
	print_graph();

	if (false)
	{
	    reduce();

	    cout << "reduced action graph" << endl;
	    print_graph();
	}

	get_order();

	commit();
    }


    ActionGraph::vertex_descriptor
    ActionGraph::add_vertex(Action::Base* action)
    {
	return boost::add_vertex(shared_ptr<Action::Base>(action), graph);
    }


    ActionGraph::edge_descriptor
    ActionGraph::add_edge(vertex_descriptor a, vertex_descriptor b)
    {
	pair<edge_descriptor, bool> tmp = boost::add_edge(a, b, graph);

	return tmp.first;
    }


    void
    ActionGraph::add_chain(vector<Action::Base*>& actions)
    {
	vertex_descriptor v1 = 0;

	for (vector<Action::Base*>::iterator it = actions.begin(); it != actions.end(); ++it)
	{
	    (*it)->first = it == actions.begin();
	    (*it)->last = it == --actions.end();

	    vertex_descriptor v2 = add_vertex(*it);

	    if (it != actions.begin())
		add_edge(v1, v2);

	    v1 = v2;
	}
    }


    vector<ActionGraph::vertex_descriptor>
    ActionGraph::huhu(sid_t sid, bool first, bool last) const
    {
	vector<ActionGraph::vertex_descriptor> ret;

	for (ActionGraph::vertex_descriptor tmp : vertices())
	{
	    if (graph[tmp]->sid == sid)
	    {
		if (first && !graph[tmp]->first)
		    continue;

		if (last && !graph[tmp]->last)
		    continue;

		ret.push_back(tmp);
	    }
	}

	return ret;
    }


    boost::iterator_range<ActionGraph::vertex_iterator>
    ActionGraph::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    void
    ActionGraph::get_actions()
    {
	set<sid_t> lhs_sids;
	for (DeviceGraph::vertex_descriptor v : lhs.vertices())
	    lhs_sids.insert(lhs.graph[v]->sid);

	set<sid_t> rhs_sids;
	for (DeviceGraph::vertex_descriptor v : rhs.vertices())
	    rhs_sids.insert(rhs.graph[v]->sid);

	vector<sid_t> created_sids;
	back_insert_iterator<vector<sid_t>> bii1(created_sids);
	set_difference(rhs_sids.begin(), rhs_sids.end(), lhs_sids.begin(), lhs_sids.end(), bii1);

	vector<sid_t> common_sids;
	back_insert_iterator<vector<sid_t>> bii2(common_sids);
	set_intersection(lhs_sids.begin(), lhs_sids.end(), rhs_sids.begin(), rhs_sids.end(), bii2);

	vector<sid_t> deleted_sids;
	back_insert_iterator<vector<sid_t>> bii3(deleted_sids);
	set_difference(lhs_sids.begin(), lhs_sids.end(), rhs_sids.begin(), rhs_sids.end(), bii3);

	for (sid_t sid : created_sids)
	{
	    DeviceGraph::vertex_descriptor v_rhs = rhs.find_vertex(sid);
	    const Device* d_rhs = rhs.graph[v_rhs].get();
	    d_rhs->add_create_actions(*this);
	}

	for (sid_t sid : common_sids)
	{
	    DeviceGraph::vertex_descriptor v_lhs = lhs.find_vertex(sid);
	    DeviceGraph::vertex_descriptor v_rhs = rhs.find_vertex(sid);

	    const BlkDevice* d_lhs = dynamic_cast<const BlkDevice*>(lhs.graph[v_lhs].get());
	    const BlkDevice* d_rhs = dynamic_cast<const BlkDevice*>(rhs.graph[v_rhs].get());

	    if ((d_lhs && d_rhs) && (d_lhs->name != d_rhs->name))
	    {
		Action::Base* action = new Action::Modify(sid);
		add_vertex(action);
	    }
	}

	for (sid_t sid : deleted_sids)
	{
	    DeviceGraph::vertex_descriptor v_lhs = lhs.find_vertex(sid);
	    const Device* d_lhs = lhs.graph[v_lhs].get();
	    d_lhs->add_delete_actions(*this);
	}
    }


    void
    ActionGraph::add_dependencies()
    {
	vector<vertex_descriptor> mounts;

	for (vertex_descriptor v : vertices())
	{
	    graph[v]->add_dependencies(v, *this);

	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(graph[v].get());
	    if (mount && mount->mount_point != "swap")
		mounts.push_back(v);
	}

	if (mounts.size() >= 2)
	{
	    // TODO correct sort
	    sort(mounts.begin(), mounts.end(), [this, &mounts](vertex_descriptor l, vertex_descriptor r) {
		const Action::Mount* ml = dynamic_cast<const Action::Mount*>(graph[l].get());
		const Action::Mount* mr = dynamic_cast<const Action::Mount*>(graph[r].get());
		return ml->mount_point <= mr->mount_point;
	    });

	    vertex_descriptor v = mounts[0];
	    for (size_t i = 1; i < mounts.size(); ++i)
	    {
		add_edge(v, mounts[i]);
		v = mounts[i];
	    }
	}
    }


    void
    ActionGraph::reduce()
    {
	graph_t reduced;

	Haha<graph_t> haha(graph);

	vector<vertex_descriptor> vertices_mapping(num_vertices(graph));

	// The function transitive_reduction is currently not documented.  Look at
	// transitive_closure for an explanation of the parameters.  It does not
	// have a vertex_copy parameter like e.g. copy_graph, instead the
	// vertices_mapping must be used for that.

	boost::transitive_reduction(graph, reduced,
				    boost::make_iterator_property_map(vertices_mapping.begin(),
								      get(boost::vertex_index, graph)),
				    haha.get());

	assert(num_vertices(reduced) == num_vertices(graph));
	assert(num_edges(reduced) <= num_edges(graph));

	for (size_t i = 0; i < num_vertices(graph); ++i)
	{
	    size_t j = vertices_mapping[i];
	    // cout << "vertices_mapping[" << i << "] = " << j << endl;
	    reduced[j] = graph[i]; // TODO check
	}

	reduced.swap(graph);
    }


    void
    ActionGraph::get_order()
    {
	try
	{
	    boost::topological_sort(graph, front_inserter(order));
	}
	catch (const boost::not_a_dag& e)
	{
	    cerr << "action graph not a DAG" << endl;
	}
    }


    void
    ActionGraph::commit() const
    {
	cout << "order" << endl;

	for (const vertex_descriptor& v : order)
	{
	    Action::Base* action = graph[v].get();
	    cout << v << " " << action->text(*this, true) << endl;
	}

	cout << endl;
    }


    void
    ActionGraph::print_graph() const
    {
	typedef map<vertex_descriptor, string> vertex_name_map_t;
	vertex_name_map_t vertex_name_map;
	boost::associative_property_map<vertex_name_map_t> my_vertex_name_map(vertex_name_map);

	for (vertex_descriptor v : vertices())
	{
	    int i = v;
	    string text = graph[v]->text(*this, false);

	    ostringstream tmp;
	    tmp << i << " [ " << text << " ]";

	    boost::put(my_vertex_name_map, v, tmp.str());
	}

	boost::print_graph(graph, my_vertex_name_map);

	cout << endl;
    }


    struct write_graph
    {
	write_graph(const ActionGraph&) {}

	void operator()(ostream& out) const
	{
	    out << "node [ style=filled ];" << endl;
	}
    };


    struct write_vertex
    {
	write_vertex(const ActionGraph& action_graph)
	    : action_graph(action_graph) {}

	const ActionGraph& action_graph;

	void operator()(ostream& out, const ActionGraph::vertex_descriptor& v) const
	{
	    const Action::Base* action = action_graph.graph[v].get();

	    string label = action->text(action_graph, false);

	    label += " [";
	    if (action->first)
		label += "f";
	    if (action->last)
		label += "l";
	    label += "]";

	    out << "[ label=\"" << label << "\"";

	    if (dynamic_cast<const Action::Nop*>(action))
		out << ", color=\"#000000\", fillcolor=\"#cccccc\"";
	    else if (dynamic_cast<const Action::Create*>(action))
		out << ", color=\"#00ff00\", fillcolor=\"#ccffcc\"";
	    else if (dynamic_cast<const Action::Modify*>(action))
		out << ", color=\"#0000ff\", fillcolor=\"#ccccff\"";
	    else if (dynamic_cast<const Action::Delete*>(action))
		out << ", color=\"#ff0000\", fillcolor=\"#ffcccc\"";
	    else
		throw logic_error("unknown Action::Base subclass");

	    out << " ]";
	}
    };


    void
    ActionGraph::write_graphviz(const string& filename) const
    {
	ofstream fout(filename + ".dot");

	fout << "// generated by libstorage" << endl;
	fout << endl;

	boost::write_graphviz(fout, graph, write_vertex(*this), boost::default_writer(),
			      write_graph(*this));

	fout.close();

	system(string("dot -Tpng < " + filename + ".dot > " + filename + ".png").c_str());
    }


    ActionGraph::simple_t
    ActionGraph::simple() const
    {
	simple_t ret;

	for (vertex_descriptor v : vertices())
	{
	    string key = graph[v]->text(*this, false);

	    vector<string> value;

	    for (edge_descriptor e : boost::make_iterator_range(out_edges(v, graph)))
		value.push_back(graph[target(e, graph)]->text(*this, false));

	    sort(value.begin(), value.end());

	    ret[key] = value;
	}

	return ret;
    }

}


namespace std
{

    ostream&
    operator<<(ostream& s, const storage::ActionGraph::simple_t& simple)
    {
	for (const pair<const string, vector<string>>& i : simple)
	{
	    s << "{ \"" << i.first << "\", { ";
	    for (const string& j : i.second)
		s << "\"" << j << "\", ";
	    s << "}," << endl;
	}

	return s;
    }

}
