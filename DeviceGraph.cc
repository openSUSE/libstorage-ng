

#include <boost/graph/copy.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>

#include "DeviceGraph.h"
#include "GraphUtils.h"


namespace storage
{

    boost::iterator_range<DeviceGraph::vertex_iterator>
    DeviceGraph::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    DeviceGraph::vertex_descriptor
    DeviceGraph::find_vertex(const string& name) const
    {
	for (vertex_descriptor v : vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(graph[v].get());
	    if (blk_device && blk_device->name == name)
		return v;
	}

	ostringstream str;
	str << "device not found, name = " << name;
	throw runtime_error(str.str());
    }


    DeviceGraph::vertex_descriptor
    DeviceGraph::find_vertex(sid_t sid) const
    {
	for (vertex_descriptor v : vertices())
	{
	    if (graph[v]->sid == sid)
		return v;
	}

	ostringstream str;
	str << "device not found, sid = " << sid;
	throw runtime_error(str.str());
    }


    Device*
    DeviceGraph::find_device(sid_t sid)
    {
	vertex_descriptor v = find_vertex(sid);

	return graph[v].get();
    }


    const Device*
    DeviceGraph::find_device(sid_t sid) const
    {
	vertex_descriptor v = find_vertex(sid);

	return graph[v].get();
    }


    bool
    DeviceGraph::vertex_exists(sid_t sid) const
    {
	try
	{
	    find_vertex(sid);
	    return true;
	}
	catch (const runtime_error& e)
	{
	    return false;
	}
    }


    DeviceGraph::vertex_descriptor
    DeviceGraph::add_vertex(Device* device)
    {
	return boost::add_vertex(shared_ptr<Device>(device), graph);
    }


    DeviceGraph::edge_descriptor
    DeviceGraph::add_edge(vertex_descriptor a, vertex_descriptor b, Holder* holder)
    {
	pair<DeviceGraph::edge_descriptor, bool> tmp = boost::add_edge(a, b, shared_ptr<Holder>(holder), graph);

	assert(tmp.second);

	return tmp.first;
    }


    void
    DeviceGraph::remove_vertex(vertex_descriptor a)
    {
	boost::clear_vertex(a, graph);
	boost::remove_vertex(a, graph);
    }


    vector<DeviceGraph::vertex_descriptor>
    DeviceGraph::siblings(vertex_descriptor v, bool itself) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp1 : boost::make_iterator_range(inv_adjacent_vertices(v, graph)))
	{
	    for (vertex_descriptor tmp2 : boost::make_iterator_range(adjacent_vertices(tmp1, graph)))
	    {
		if (itself || v != tmp2)
		    ret.push_back(tmp2);
	    }
	}

	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end()), ret.end());

	return ret;
    }


    vector<DeviceGraph::vertex_descriptor>
    DeviceGraph::descendants(vertex_descriptor v, bool itself) const
    {
	vector<vertex_descriptor> ret;

	Haha<graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(false, ret);

	boost::breadth_first_search(graph, v, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), v), ret.end());

	return ret;
    }


    vector<DeviceGraph::vertex_descriptor>
    DeviceGraph::ancestors(vertex_descriptor v, bool itself) const
    {
	vector<vertex_descriptor> ret;

	typedef boost::reverse_graph<graph_t> reverse_graph_t;

	reverse_graph_t reverse_graph(graph);

	Haha<reverse_graph_t> haha(reverse_graph);

	vertex_recorder<vertex_descriptor> vis(false, ret);

	boost::breadth_first_search(reverse_graph, v, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), v), ret.end());

	return ret;
    }


    vector<DeviceGraph::vertex_descriptor>
    DeviceGraph::leafs(vertex_descriptor v, bool itself) const
    {
	vector<vertex_descriptor> ret;

	Haha<graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(true, ret);

	boost::breadth_first_search(graph, v, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), v), ret.end());

	return ret;
    }


    vector<DeviceGraph::vertex_descriptor>
    DeviceGraph::roots(vertex_descriptor v, bool itself) const
    {
	vector<vertex_descriptor> ret;

	typedef boost::reverse_graph<graph_t> reverse_graph_t;

	reverse_graph_t reverse_graph(graph);

	Haha<reverse_graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(true, ret);

	boost::breadth_first_search(reverse_graph, v, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), v), ret.end());

	return ret;
    }


    void
    DeviceGraph::check() const
    {
	{
	    // look for cycles

	    Haha<graph_t> haha(graph);

	    bool has_cycle = false;

	    cycle_detector vis(has_cycle);
	    boost::depth_first_search(graph, visitor(vis).vertex_index_map(haha.get()));

	    if (has_cycle)
		cerr << "graph has a cycle" << endl;
	}

	{
	    // check uniqueness of sid and name

	    set<sid_t> sids;
	    set<string> names;

	    for (vertex_descriptor v : vertices())
	    {
		const Device* device = graph[v].get();

		sid_t sid = device->sid;
		pair<set<sid_t>::iterator, bool> tmp = sids.insert(sid);
		if (!tmp.second)
		    cerr << "sid not unique" << endl;

		const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(device);
		if (blk_device)
		{
		    pair<set<string>::iterator, bool> tmp = names.insert(blk_device->name);
		    if (!tmp.second)
			cerr << "name not unique" << endl;
		}
	    }
	}

	{
	    for (vertex_descriptor v : vertices())
	    {
		graph[v]->check();
	    }
	}

	// TODO check parallel edges if not using boost::setS

	// TODO check that out-edges are consistent, e.g. of same type, only one per Subdevice
	// TODO check that in-edges are consistent, e.g. of same type, exactly one for Partition
	// in general subcheck for each device
    }


    void
    DeviceGraph::copy(DeviceGraph& dest) const
    {
	dest.graph.clear();

	Haha<graph_t> haha(graph);

	CloneCopier<graph_t> copier(graph, dest.graph);

	boost::copy_graph(graph, dest.graph, vertex_index_map(haha.get()).vertex_copy(copier));
    }


    void
    DeviceGraph::print_graph() const
    {
	typedef map<vertex_descriptor, string> vertex_name_map_t;
	vertex_name_map_t vertex_name_map;
	boost::associative_property_map<vertex_name_map_t> my_vertex_name_map(vertex_name_map);

	for (vertex_descriptor v : vertices())
	{
	    sid_t sid = graph[v]->sid;
	    string label = graph[v]->display_name();

	    ostringstream tmp;
	    tmp << sid << " [ " << label << " ]";

	    boost::put(my_vertex_name_map, v, tmp.str());
	}

	boost::print_graph(graph, my_vertex_name_map);

	cout << endl;
    }


    struct write_graph
    {
	write_graph(const DeviceGraph&) {}

	void operator()(ostream& out) const
	{
	    out << "node [ shape=rectangle, style=filled, fontname=\"Arial\" ];" << endl;
	    out << "edge [ color=\"#444444\" ];" << endl;
	}
    };


    struct write_vertex
    {
	write_vertex(const DeviceGraph& device_graph) : device_graph(device_graph) {}

	const DeviceGraph& device_graph;

	void operator()(ostream& out, const DeviceGraph::vertex_descriptor& v) const
	{
	    const Device* device = device_graph.graph[v].get();

	    out << "[ label=\"" << device->sid << " " << device->display_name() << "\"";

	    if (dynamic_cast<const Disk*>(device))
		out << ", color=\"#ff0000\", fillcolor=\"#ffaaaa\"";
	    else if (dynamic_cast<const Partition*>(device))
		out << ", color=\"#cc33cc\", fillcolor=\"#eeaaee\"";
	    else if (dynamic_cast<const LvmVg*>(device))
		out << ", color=\"#0000ff\", fillcolor=\"#aaaaff\"";
	    else if (dynamic_cast<const LvmLv*>(device))
		out << ", color=\"#6622dd\", fillcolor=\"#bb99ff\"";
	    else if (dynamic_cast<const Encryption*>(device))
		out << ", color=\"#6622dd\", fillcolor=\"#bb99ff\"";
	    else if (dynamic_cast<const Filesystem*>(device))
		out << ", color=\"#008800\", fillcolor=\"#99ee99\"";
	    else
		throw logic_error("unknown Device subclass");

	    out << " ]";
	}
    };


    struct write_edge
    {
	write_edge(const DeviceGraph& device_graph) : device_graph(device_graph) {}

	const DeviceGraph& device_graph;

	void operator()(ostream& out, const DeviceGraph::edge_descriptor& e) const
	{
	    const Holder* holder = device_graph.graph[e].get();

	    if (dynamic_cast<const Subdevice*>(holder))
		out << "[ style=solid ]";
	    else if (dynamic_cast<const Using*>(holder))
		out << "[ style=dotted ]";
	    else if (dynamic_cast<const Filesystem*>(holder))
		out << "[ style=dashed ]";
	    else
		throw logic_error("unknown Holder subclass");
	}
    };


    void
    DeviceGraph::write_graphviz(const string& filename) const
    {
	ofstream fout(filename + ".dot");

	Haha<graph_t> haha(graph);

	fout << "// generated by libstorage" << endl;
	fout << endl;

	// TODO write same rank stuff, should be possible in write_graph

	// TODO the node must include device name (or better some unique id) to
	// detect clicked objects in YaST

	// TODO in the long run a filesystem must support several mount points, so
	// boost::write_graphviz might not be albe to handle our needs (or the
	// needs of YaST).  Just keep a write_graphviz function here for debugging
	// and move the thing YaST needs to yast2-storage.

	boost::write_graphviz(fout, graph, write_vertex(*this), write_edge(*this),
			      write_graph(*this), haha.get());

	fout.close();

	system(string("dot -Tpng < " + filename + ".dot > " + filename + ".png").c_str());
    }

}
