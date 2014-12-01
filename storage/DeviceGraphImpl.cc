

#include <boost/graph/copy.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>

#include "storage/DeviceGraph.h"
#include "storage/GraphUtils.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Filesystem.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    bool
    DeviceGraph::Impl::empty() const
    {
	return boost::num_vertices(graph) == 0;
    }


    size_t
    DeviceGraph::Impl::numVertices() const
    {
	return boost::num_vertices(graph);
    }


    size_t
    DeviceGraph::Impl::numEdges() const
    {
	return boost::num_edges(graph);
    }


    boost::iterator_range<DeviceGraph::Impl::vertex_iterator>
    DeviceGraph::Impl::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    boost::iterator_range<DeviceGraph::Impl::edge_iterator>
    DeviceGraph::Impl::edges() const
    {
	return boost::make_iterator_range(boost::edges(graph));
    }


    set<sid_t>
    DeviceGraph::Impl::getSids() const
    {
	set<sid_t> sids;
	for (vertex_descriptor vertex : vertices())
	    sids.insert(graph[vertex]->getSid());
	return sids;
    }


    DeviceGraph::Impl::vertex_descriptor
    DeviceGraph::Impl::find_vertex(const string& name) const
    {
	for (vertex_descriptor v : vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(graph[v].get());
	    if (blk_device && blk_device->getName() == name)
		return v;
	}

	ostringstream str;
	str << "device not found, name = " << name;
	throw runtime_error(str.str());
    }


    DeviceGraph::Impl::vertex_descriptor
    DeviceGraph::Impl::find_vertex(sid_t sid) const
    {
	for (vertex_descriptor v : vertices())
	{
	    if (graph[v]->getSid() == sid)
		return v;
	}

	ostringstream str;
	str << "device not found, sid = " << sid;
	throw runtime_error(str.str());
    }


    void
    DeviceGraph::Impl::remove_vertex(vertex_descriptor vertex)
    {
	boost::clear_vertex(vertex, graph);
	boost::remove_vertex(vertex, graph);
    }


    void
    DeviceGraph::Impl::swap(DeviceGraph::Impl& x)
    {
	graph.swap(x.graph);
    }


    size_t
    DeviceGraph::Impl::num_children(vertex_descriptor vertex) const
    {
	return boost::out_degree(vertex, graph);
    }


    size_t
    DeviceGraph::Impl::num_parents(vertex_descriptor vertex) const
    {
	return boost::in_degree(vertex, graph);
    }


    vector<DeviceGraph::Impl::vertex_descriptor>
    DeviceGraph::Impl::children(vertex_descriptor vertex) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp : boost::make_iterator_range(boost::adjacent_vertices(vertex, graph)))
	    ret.push_back(tmp);

	return ret;
    }


    vector<DeviceGraph::Impl::vertex_descriptor>
    DeviceGraph::Impl::parents(vertex_descriptor vertex) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp : boost::make_iterator_range(boost::inv_adjacent_vertices(vertex, graph)))
	    ret.push_back(tmp);

	return ret;
    }


    vector<DeviceGraph::Impl::vertex_descriptor>
    DeviceGraph::Impl::siblings(vertex_descriptor vertex, bool itself) const
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


    vector<DeviceGraph::Impl::vertex_descriptor>
    DeviceGraph::Impl::descendants(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	Haha<graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(false, ret);

	boost::breadth_first_search(graph, vertex, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<DeviceGraph::Impl::vertex_descriptor>
    DeviceGraph::Impl::ancestors(vertex_descriptor vertex, bool itself) const
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


    vector<DeviceGraph::Impl::vertex_descriptor>
    DeviceGraph::Impl::leafs(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	Haha<graph_t> haha(graph);

	vertex_recorder<vertex_descriptor> vis(true, ret);

	boost::breadth_first_search(graph, vertex, visitor(vis).vertex_index_map(haha.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<DeviceGraph::Impl::vertex_descriptor>
    DeviceGraph::Impl::roots(vertex_descriptor vertex, bool itself) const
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


    void
    DeviceGraph::Impl::save() const
    {
	string fname("device-graph.info.tmp");

	XmlFile xml;

	xmlNode* node = xmlNewNode("DeviceGraph");
	xml.setRootElement(node);

	xmlNode* devices = xmlNewChild(node, "Devices");

	for (vertex_descriptor vertex : vertices())
	{
	    const Device::Impl& device_impl = graph[vertex]->getImpl();

	    xmlNode* subnode = xmlNewChild(devices, device_impl.getClassName());

	    setProp(subnode, "sid", device_impl.getSid());

	    device_impl.save(subnode);
	}

	/*
	xmlNode* holders = xmlNewChild(node, "Holders");

	for (edge_descriptor edge : edges())
	{
	    const Holder::Impl& holder_impl = graph[edge]->getImpl();
	    xmlNode* subnode = xmlNewChild(holders, holder_impl.getClassName());

	    setProp(subnode, "source-sid", holder_impl.getSourceSid());
	    setProp(subnode, "target-sid", holder_impl.getTargetSid());

	    holder_impl.save(subnode);
	}
	*/

	xml.save(fname);
    }


    void
    DeviceGraph::Impl::print_graph() const
    {
	typedef map<vertex_descriptor, string> vertex_name_map_t;
	vertex_name_map_t vertex_name_map;
	boost::associative_property_map<vertex_name_map_t> my_vertex_name_map(vertex_name_map);

	for (vertex_descriptor v : vertices())
	{
	    sid_t sid = graph[v]->getSid();
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
	write_graph(const DeviceGraph::Impl&) {}

	void operator()(ostream& out) const
	{
	    out << "node [ shape=rectangle, style=filled, fontname=\"Arial\" ];" << endl;
	    out << "edge [ color=\"#444444\" ];" << endl;
	}
    };


    struct write_vertex
    {
	write_vertex(const DeviceGraph::Impl& device_graph) : device_graph(device_graph) {}

	const DeviceGraph::Impl& device_graph;

	void operator()(ostream& out, const DeviceGraph::Impl::vertex_descriptor& v) const
	{
	    const Device* device = device_graph.graph[v].get();

	    out << "[ label=\"" << device->getSid() << " " << device->display_name() << "\"";

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
	write_edge(const DeviceGraph::Impl& device_graph) : device_graph(device_graph) {}

	const DeviceGraph::Impl& device_graph;

	void operator()(ostream& out, const DeviceGraph::Impl::edge_descriptor& e) const
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
    DeviceGraph::Impl::write_graphviz(const string& filename) const
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
