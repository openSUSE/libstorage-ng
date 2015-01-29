

#include <boost/graph/copy.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Action.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"


namespace storage
{

    Actiongraph::Actiongraph(const Storage& storage, const Devicegraph* lhs, const Devicegraph* rhs)
	: storage(storage), lhs(lhs), rhs(rhs)
    {
	cout << "lhs graph" << endl
	     << *lhs << endl;
	lhs->check();

	cout << "rhs graph" << endl
	     << *rhs << endl;
	rhs->check();

	get_actions();
	add_dependencies();

	cout << "action graph" << endl;
	print_graph();

	get_order();
    }


    Actiongraph::vertex_descriptor
    Actiongraph::add_vertex(Action::Base* action)
    {
	return boost::add_vertex(shared_ptr<Action::Base>(action), graph);
    }


    Actiongraph::edge_descriptor
    Actiongraph::add_edge(vertex_descriptor a, vertex_descriptor b)
    {
	pair<edge_descriptor, bool> tmp = boost::add_edge(a, b, graph);

	return tmp.first;
    }


    void
    Actiongraph::add_chain(vector<Action::Base*>& actions)
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


    vector<Actiongraph::vertex_descriptor>
    Actiongraph::huhu(sid_t sid, bool first, bool last) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp : vertices())
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


    boost::iterator_range<Actiongraph::vertex_iterator>
    Actiongraph::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    void
    Actiongraph::get_actions()
    {
	const set<sid_t> lhs_sids = lhs->get_impl().get_device_sids();
	const set<sid_t> rhs_sids = rhs->get_impl().get_device_sids();

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
	    Devicegraph::Impl::vertex_descriptor v_rhs = rhs->get_impl().find_vertex(sid);
	    const Device* d_rhs = rhs->get_impl().graph[v_rhs].get();
	    d_rhs->get_impl().add_create_actions(*this);
	}

	for (sid_t sid : common_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    Devicegraph::Impl::vertex_descriptor v_rhs = rhs->get_impl().find_vertex(sid);

	    const BlkDevice* d_lhs = to_blkdevice(lhs->get_impl().graph[v_lhs].get());
	    const BlkDevice* d_rhs = to_blkdevice(rhs->get_impl().graph[v_rhs].get());

	    if ((d_lhs && d_rhs) && (d_lhs->get_name() != d_rhs->get_name()))
	    {
		Action::Base* action = new Action::Modify(sid);
		add_vertex(action);
	    }
	}

	for (sid_t sid : deleted_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    const Device* d_lhs = lhs->get_impl().graph[v_lhs].get();
	    d_lhs->get_impl().add_delete_actions(*this);
	}
    }


    void
    Actiongraph::add_dependencies()
    {
	vector<vertex_descriptor> mounts;

	for (vertex_descriptor v : vertices())
	{
	    graph[v]->add_dependencies(v, *this);

	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(graph[v].get());
	    if (mount && mount->mountpoint != "swap")
		mounts.push_back(v);
	}

	if (mounts.size() >= 2)
	{
	    // TODO correct sort
	    sort(mounts.begin(), mounts.end(), [this, &mounts](vertex_descriptor l, vertex_descriptor r) {
		const Action::Mount* ml = dynamic_cast<const Action::Mount*>(graph[l].get());
		const Action::Mount* mr = dynamic_cast<const Action::Mount*>(graph[r].get());
		return ml->mountpoint <= mr->mountpoint;
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
    Actiongraph::get_order()
    {
	try
	{
	    boost::topological_sort(graph, front_inserter(order));
	}
	catch (const boost::not_a_dag&)
	{
	    cerr << "action graph not a DAG" << endl;
	}

	cout << "order" << endl;

	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();
	    cout << vertex << " " << action->text(*this, false).native << endl;
	}

	cout << endl;
    }


    list<string>
    Actiongraph::get_commit_steps() const
    {
	list<string> commit_steps;

	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();
	    string text = action->text(*this, true).text;

	    commit_steps.push_back(text);
	}

	return commit_steps;
    }


    void
    Actiongraph::commit(const CommitCallbacks* commit_callbacks) const
    {
	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();
	    string text = action->text(*this, true).text;

	    y2mil("Commit Action " << text);
	    cout << text << endl;

	    if (commit_callbacks)
		commit_callbacks->message(text);

	    action->commit(*this);
	}
    }


    void
    Actiongraph::print_graph() const
    {
	typedef map<vertex_descriptor, string> vertex_name_map_t;
	vertex_name_map_t vertex_name_map;
	boost::associative_property_map<vertex_name_map_t> my_vertex_name_map(vertex_name_map);

	for (vertex_descriptor v : vertices())
	{
	    int i = v;
	    string text = graph[v]->text(*this, false).text;

	    ostringstream tmp;
	    tmp << i << " [ " << text << " ]";

	    boost::put(my_vertex_name_map, v, tmp.str());
	}

	boost::print_graph(graph, my_vertex_name_map);

	cout << endl;
    }


    struct write_graph
    {
	write_graph(const Actiongraph&) {}

	void operator()(ostream& out) const
	{
	    out << "node [ style=filled ];" << endl;
	}
    };


    struct write_vertex
    {
	write_vertex(const Actiongraph& actiongraph)
	    : actiongraph(actiongraph) {}

	const Actiongraph& actiongraph;

	void operator()(ostream& out, const Actiongraph::vertex_descriptor& v) const
	{
	    const Action::Base* action = actiongraph.graph[v].get();

	    string label = action->text(actiongraph, false).text;

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
    Actiongraph::write_graphviz(const string& filename) const
    {
	ofstream fout(filename);

	fout << "// generated by libstorage" << endl;
	fout << endl;

	boost::write_graphviz(fout, graph, write_vertex(*this), boost::default_writer(),
			      write_graph(*this));

	fout.close();
    }


    Actiongraph::simple_t
    Actiongraph::get_simple() const
    {
	simple_t ret;

	for (vertex_descriptor v : vertices())
	{
	    string key = graph[v]->text(*this, false).native;

	    vector<string> value;

	    for (edge_descriptor e : boost::make_iterator_range(out_edges(v, graph)))
		value.push_back(graph[target(e, graph)]->text(*this, false).native);

	    sort(value.begin(), value.end());

	    ret[key] = value;
	}

	return ret;
    }

}


namespace std
{

    ostream&
    operator<<(ostream& s, const storage::Actiongraph::simple_t& simple)
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
