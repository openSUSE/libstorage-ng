

#include <boost/algorithm/string.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

#include "config.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Action.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Utils/StorageTmpl.h"


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
	set_special_actions();
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
	    const Device* d_rhs = rhs->get_impl().graph[v_rhs].get();

	    d_rhs->get_impl().add_create_actions(*this);
	}

	for (sid_t sid : common_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    const Device* d_lhs = lhs->get_impl().graph[v_lhs].get();

	    Devicegraph::Impl::vertex_descriptor v_rhs = rhs->get_impl().find_vertex(sid);
	    const Device* d_rhs = rhs->get_impl().graph[v_rhs].get();

	    d_rhs->get_impl().add_modify_actions(*this, d_lhs);
	}

	for (sid_t sid : deleted_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    const Device* d_lhs = lhs->get_impl().graph[v_lhs].get();

	    d_lhs->get_impl().add_delete_actions(*this);
	}
    }


    void
    Actiongraph::set_special_actions()
    {
	boost::iterator_range<vertex_iterator> range = vertices();

	mount_root_filesystem = range.end();

	for (vertex_iterator it = range.begin(); it != range.end(); ++it)
	{
	    const Action::Base* action = graph[*it].get();

	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(action);
	    if (mount && mount->mountpoint == "/")
		mount_root_filesystem = it;
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
	    if (dynamic_cast<const Action::Nop*>(action))
		continue;

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
	    if (dynamic_cast<const Action::Nop*>(action))
		continue;

	    string text = action->text(*this, true).text;

	    y2mil("Commit Action " << text);
	    cout << text << endl;

	    if (commit_callbacks)
	    {
		commit_callbacks->message(text);
		commit_callbacks->pre(action);
	    }

	    try
	    {
		action->commit(*this);
	    }
	    catch (const exception& e)
	    {
		if (!commit_callbacks || !commit_callbacks->error(text, e.what()))
		    throw;
	    }

	    if (commit_callbacks)
	    {
		commit_callbacks->post(action);
	    }
	}
    }


    Text
    Actiongraph::get_action_text(vertex_descriptor v, bool doing) const
    {
	return graph[v]->text(*this, doing);
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
	    out << "node [ shape=rectangle, style=filled ];" << endl;
	}
    };


    struct write_vertex
    {
	write_vertex(const Actiongraph& actiongraph, bool details)
	    : actiongraph(actiongraph), details(details) {}

	const Actiongraph& actiongraph;
	const bool details;

	void operator()(ostream& out, const Actiongraph::vertex_descriptor& v) const
	{
	    const Action::Base* action = actiongraph.graph[v].get();

	    string label = action->text(actiongraph, false).text;

	    if (details)
	    {
		label += "\n" "sid:" + decString(action->sid);

		label += " [";
		if (action->first)
		    label += "f";
		if (action->last)
		    label += "l";
		label += "]";
	    }

	    out << "[ label=" << boost::escape_dot_string(label);

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
    Actiongraph::write_graphviz(const string& filename, bool details) const
    {
	ofstream fout(filename);

	fout << "// generated by libstorage version " VERSION << endl;
	fout << endl;

	boost::write_graphviz(fout, graph, write_vertex(*this, details), boost::default_writer(),
			      write_graph(*this));

	fout.close();
    }


    Cmp::Cmp(const Actiongraph& actiongraph, const expected_t& expected)
    {
	for (const string& line : expected)
	    entries.push_back(Entry(line));

	check();

	cmp_texts(actiongraph);

	if (!ok())
	    return;

	cmp_dependencies(actiongraph);
    }


    Cmp::Entry::Entry(const string& line)
    {
	string::size_type pos1 = line.find('-');
	if (pos1 == string::npos)
	    throw runtime_error("parse error");

	string::size_type pos2 = line.rfind("->");
	if (pos2 == string::npos)
	    throw runtime_error("parse error");

	id = boost::trim_copy(line.substr(0, pos1), locale::classic());
	text = boost::trim_copy(line.substr(pos1 + 1, pos2 - pos1 - 1), locale::classic());

	string tmp = boost::trim_copy(line.substr(pos2 + 2), locale::classic());
	if (!tmp.empty())
	    boost::split(dep_ids, tmp, boost::is_any_of(" "), boost::token_compress_on);
    }


    void
    Cmp::check() const
    {
	set<string> ids;
	set<string> texts;

	for (const Entry& entry : entries)
	{
	    if (!ids.insert(entry.id).second)
		throw runtime_error("duplicate id");

	    if (!texts.insert(entry.text).second)
		throw runtime_error("duplicate text");
	}

	for (const Entry& entry : entries)
	{
	    for (const string dep_id : entry.dep_ids)
	    {
		if (ids.find(dep_id) == ids.end())
		    throw runtime_error("unknown dependency-id");
	    }
	}
    }


    void
    Cmp::cmp_texts(const Actiongraph& actiongraph)
    {
	set<string> tmp1;
	for (Actiongraph::vertex_descriptor v : actiongraph.vertices())
	    tmp1.insert(actiongraph.get_action_text(v, false).native);

	set<string> tmp2;
	for (const Entry& entry : entries)
	    tmp2.insert(entry.text);

	if (tmp1 != tmp2)
	{
	    errors.push_back("action texts differ");

	    vector<string> diff1;
	    set_difference(tmp2.begin(), tmp2.end(), tmp1.begin(), tmp1.end(), back_inserter(diff1));
	    for (const string& error : diff1)
		errors.push_back("- " + error);

	    vector<string> diff2;
	    set_difference(tmp1.begin(), tmp1.end(), tmp2.begin(), tmp2.end(), back_inserter(diff2));
	    for (const string& error : diff2)
		errors.push_back("+ " + error);
	}
    }


    void
    Cmp::cmp_dependencies(const Actiongraph& actiongraph)
    {
	map<string, string> text_to_id;
	for (const Entry& entry : entries)
	    text_to_id[entry.text] = entry.id;

	map<string, Actiongraph::vertex_descriptor> text_to_v;
	for (Actiongraph::vertex_descriptor v : actiongraph.vertices())
	    text_to_v[actiongraph.get_action_text(v, false).native] = v;

	for (const Entry& entry : entries)
	{
	    Actiongraph::vertex_descriptor v = text_to_v[entry.text];

	    set<string> tmp;
	    for (Actiongraph::edge_descriptor e : boost::make_iterator_range(out_edges(v, actiongraph.graph)))
	    {
		string text = actiongraph.get_action_text(target(e, actiongraph.graph), false).native;
		tmp.insert(text_to_id[text]);
	    }

	    if (tmp != entry.dep_ids)
	    {
		errors.push_back("wrong dependencies for '" + entry.text + "'");
		errors.push_back("- " + boost::join(tmp, " "));
		errors.push_back("+ " + boost::join(entry.dep_ids, " "));
	    }
	}
    }


    std::ostream&
    operator<<(std::ostream& out, const Cmp& cmp)
    {
	out << endl;

	for (const string& error : cmp.errors)
	    out << error << endl;

	return out;
    }

}
