

#include <boost/graph/copy.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>

#include "storage/DevicegraphImpl.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Utils/XmlFile.h"


namespace storage_bgl
{

    bool
    Devicegraph::Impl::empty() const
    {
	return boost::num_vertices(graph) == 0;
    }


    size_t
    Devicegraph::Impl::num_devices() const
    {
	return boost::num_vertices(graph);
    }


    size_t
    Devicegraph::Impl::num_holders() const
    {
	return boost::num_edges(graph);
    }


    boost::iterator_range<Devicegraph::Impl::vertex_iterator>
    Devicegraph::Impl::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    boost::iterator_range<Devicegraph::Impl::edge_iterator>
    Devicegraph::Impl::edges() const
    {
	return boost::make_iterator_range(boost::edges(graph));
    }


    set<sid_t>
    Devicegraph::Impl::get_sids() const
    {
	set<sid_t> sids;
	for (vertex_descriptor vertex : vertices())
	    sids.insert(graph[vertex]->get_sid());
	return sids;
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::find_vertex(sid_t sid) const
    {
	for (vertex_descriptor v : vertices())
	{
	    if (graph[v]->get_sid() == sid)
		return v;
	}

	ostringstream str;
	str << "device not found, sid = " << sid;
	throw DeviceNotFound(str.str());
    }


    void
    Devicegraph::Impl::remove_vertex(vertex_descriptor vertex)
    {
	boost::clear_vertex(vertex, graph);
	boost::remove_vertex(vertex, graph);
    }


    void
    Devicegraph::Impl::swap(Devicegraph::Impl& x)
    {
	graph.swap(x.graph);
    }


    size_t
    Devicegraph::Impl::num_children(vertex_descriptor vertex) const
    {
	return boost::out_degree(vertex, graph);
    }


    size_t
    Devicegraph::Impl::num_parents(vertex_descriptor vertex) const
    {
	return boost::in_degree(vertex, graph);
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::children(vertex_descriptor vertex) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp : boost::make_iterator_range(boost::adjacent_vertices(vertex, graph)))
	    ret.push_back(tmp);

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::parents(vertex_descriptor vertex) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp : boost::make_iterator_range(boost::inv_adjacent_vertices(vertex, graph)))
	    ret.push_back(tmp);

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::siblings(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp1 : boost::make_iterator_range(inv_adjacent_vertices(vertex, graph)))
	{
	    for (vertex_descriptor tmp2 : boost::make_iterator_range(adjacent_vertices(tmp1, graph)))
	    {
		if (itself || vertex != tmp2)
		    ret.push_back(tmp2);
	    }
	}

	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end()), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::descendants(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	Haha<graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(false, ret);

	boost::breadth_first_search(graph, vertex, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::ancestors(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	typedef boost::reverse_graph<graph_t> reverse_graph_t;

	reverse_graph_t reverse_graph(graph);

	Haha<reverse_graph_t> haha(reverse_graph);

	vertex_recorder<vertex_descriptor> vis(false, ret);

	boost::breadth_first_search(reverse_graph, vertex, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::leafs(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	Haha<graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(true, ret);

	boost::breadth_first_search(graph, vertex, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::roots(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	typedef boost::reverse_graph<graph_t> reverse_graph_t;

	reverse_graph_t reverse_graph(graph);

	Haha<reverse_graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(true, ret);

	boost::breadth_first_search(reverse_graph, vertex, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    typedef Device* (*device_load_fnc)(Devicegraph* devicegraph, const xmlNode* node);


    const map<string, device_load_fnc> device_load_registry = {
	{ "Disk", (device_load_fnc)(&Disk::load) },
	{ "Gpt", (device_load_fnc)(&Gpt::load) },
	{ "Partition", (device_load_fnc)(&Partition::load) },
	{ "LvmVg", (device_load_fnc)(&LvmVg::load) },
	{ "LvmLv", (device_load_fnc)(&LvmLv::load) },
	{ "Encryption", (device_load_fnc)(&Encryption::load) },
	{ "Ext4", (device_load_fnc)(&Ext4::load) },
	{ "Swap", (device_load_fnc)(&Swap::load) },
    };


    typedef Holder* (*holder_load_fnc)(Devicegraph* devicegraph, const xmlNode* node);


    const map<string, holder_load_fnc> holder_load_registry = {
	{ "Using", (holder_load_fnc)(&Using::load) },
	{ "Subdevice", (holder_load_fnc)(&Subdevice::load) },
    };


    void
    Devicegraph::Impl::load(Devicegraph* devicegraph, const string& filename)
    {
	XmlFile xml(filename);

	const xmlNode* root_node = xml.getRootElement();

	const xmlNode* devicegraph_node = getChildNode(root_node, "Devicegraph");
	assert(devicegraph_node);

	const xmlNode* devices_node = getChildNode(devicegraph_node, "Devices");
	assert(devices_node);

	for (const xmlNode* device_node : getChildNodes(devices_node))
	{
	    const string& class_name = (const char*) device_node->parent->name;

	    map<string, device_load_fnc>::const_iterator it = device_load_registry.find(class_name);
	    if (it == device_load_registry.end())
		throw runtime_error("unknown device class name");

	    it->second(devicegraph, device_node);
	}

	const xmlNode* holders_node = getChildNode(devicegraph_node, "Holders");
	assert(holders_node);

	for (const xmlNode* holder_node : getChildNodes(holders_node))
	{
	    const string& class_name = (const char*) holder_node->parent->name;

	    map<string, holder_load_fnc>::const_iterator it = holder_load_registry.find(class_name);
	    if (it == holder_load_registry.end())
		throw runtime_error("unknown holder class name");

	    it->second(devicegraph, holder_node);
	}
    }


    void
    Devicegraph::Impl::save(const string& filename) const
    {
	XmlFile xml;

	xmlNode* devicegraph_node = xmlNewNode("Devicegraph");
	xml.setRootElement(devicegraph_node);

	xmlNode* devices_node = xmlNewChild(devicegraph_node, "Devices");

	for (vertex_descriptor vertex : vertices())
	{
	    const Device* device = graph[vertex].get();
	    xmlNode* device_node = xmlNewChild(devices_node, device->get_classname());
	    device->save(device_node);
	}

	xmlNode* holders_node = xmlNewChild(devicegraph_node, "Holders");

	for (edge_descriptor edge : edges())
	{
	    const Holder* holder = graph[edge].get();
	    xmlNode* holder_node = xmlNewChild(holders_node, holder->get_classname());
	    holder->save(holder_node);
	}

	xml.save(filename);
    }


    void
    Devicegraph::Impl::print_graph() const
    {
	typedef map<vertex_descriptor, string> vertex_name_map_t;
	vertex_name_map_t vertex_name_map;
	boost::associative_property_map<vertex_name_map_t> my_vertex_name_map(vertex_name_map);

	for (vertex_descriptor v : vertices())
	{
	    sid_t sid = graph[v]->get_sid();
	    string label = graph[v]->get_displayname();

	    ostringstream tmp;
	    tmp << sid << " [ " << label << " ]";

	    boost::put(my_vertex_name_map, v, tmp.str());
	}

	boost::print_graph(graph, my_vertex_name_map);

	cout << endl;
    }


    struct write_graph
    {
	write_graph(const Devicegraph::Impl&) {}

	void operator()(ostream& out) const
	{
	    out << "node [ shape=rectangle, style=filled, fontname=\"Arial\" ];" << endl;
	    out << "edge [ color=\"#444444\" ];" << endl;
	}
    };


    struct write_vertex
    {
	write_vertex(const Devicegraph::Impl& devicegraph) : devicegraph(devicegraph) {}

	const Devicegraph::Impl& devicegraph;

	void operator()(ostream& out, const Devicegraph::Impl::vertex_descriptor& v) const
	{
	    const Device* device = devicegraph.graph[v].get();

	    out << "[ label=\"" << device->get_sid() << " " << device->get_displayname() << "\"";

	    if (dynamic_cast<const Disk*>(device))
		out << ", color=\"#ff0000\", fillcolor=\"#ffaaaa\"";
	    else if (dynamic_cast<const PartitionTable*>(device))
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
	write_edge(const Devicegraph::Impl& devicegraph) : devicegraph(devicegraph) {}

	const Devicegraph::Impl& devicegraph;

	void operator()(ostream& out, const Devicegraph::Impl::edge_descriptor& e) const
	{
	    const Holder* holder = devicegraph.graph[e].get();

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
    Devicegraph::Impl::write_graphviz(const string& filename) const
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
