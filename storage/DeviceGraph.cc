

#include <boost/graph/copy.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>

#include "storage/DeviceGraph.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Filesystem.h"
#include "storage/Holders/HolderImpl.h"


namespace storage
{

    DeviceGraph::DeviceGraph()
	: impl(new Impl())
    {
    }


    DeviceGraph::~DeviceGraph()
    {
    }


    void
    DeviceGraph::load(const string& filename)
    {
	getImpl().load(this, filename);
    }


    void
    DeviceGraph::save(const string& filename) const
    {
	getImpl().save(filename);
    }


    bool
    DeviceGraph::empty() const
    {
	return getImpl().empty();
    }


    size_t
    DeviceGraph::numDevices() const
    {
	return getImpl().numDevices();
    }


    size_t
    DeviceGraph::numHolders() const
    {
	return getImpl().numHolders();
    }


    class CloneCopier
    {

    public:

	CloneCopier(const DeviceGraph& g_in, DeviceGraph& g_out)
	    : g_in(g_in), g_out(g_out) {}

	void operator()(const DeviceGraph::Impl::vertex_descriptor& v_in,
			DeviceGraph::Impl::vertex_descriptor& v_out)
	{
	    g_out.getImpl().graph[v_out].reset(g_in.getImpl().graph[v_in]->clone());

	    Device* d_out = g_out.getImpl().graph[v_out].get();
	    d_out->getImpl().setDeviceGraphAndVertex(&g_out, v_out);
	}

	void operator()(const DeviceGraph::Impl::edge_descriptor& e_in,
			DeviceGraph::Impl::edge_descriptor& e_out)
	{
	    g_out.getImpl().graph[e_out].reset(g_in.getImpl().graph[e_in]->clone());

	    Holder* h_out = g_out.getImpl().graph[e_out].get();
	    h_out->getImpl().setDeviceGraphAndEdge(&g_out, e_out);
	}

    private:

	const DeviceGraph& g_in;
	DeviceGraph& g_out;

    };


    Device*
    DeviceGraph::findDevice(sid_t sid)
    {
	Impl::vertex_descriptor v = getImpl().find_vertex(sid);

	return getImpl().graph[v].get();
    }


    const Device*
    DeviceGraph::findDevice(sid_t sid) const
    {
	Impl::vertex_descriptor v = getImpl().find_vertex(sid);

	return getImpl().graph[v].get();
    }


    bool
    DeviceGraph::vertex_exists(sid_t sid) const
    {
	try
	{
	    getImpl().find_vertex(sid);
	    return true;
	}
	catch (const runtime_error& e)
	{
	    return false;
	}
    }


    Holder*
    DeviceGraph::findHolder(sid_t source_sid, sid_t target_sid)
    {
	for (Impl::edge_descriptor edge : getImpl().edges())
	{
	    if (getImpl().graph[source(edge, getImpl().graph)]->getSid() == source_sid &&
		getImpl().graph[target(edge, getImpl().graph)]->getSid() == target_sid)
		return getImpl().graph[edge].get();
	}

	ostringstream str;
	str << "holder not found, source_sid = " << source_sid << ", target_sid = " << target_sid;
	throw HolderNotFound(str.str());
    }


    const Holder*
    DeviceGraph::findHolder(sid_t source_sid, sid_t target_sid) const
    {
	for (Impl::edge_descriptor edge : getImpl().edges())
	{
	    if (getImpl().graph[source(edge, getImpl().graph)]->getSid() == source_sid &&
		getImpl().graph[target(edge, getImpl().graph)]->getSid() == target_sid)
		return getImpl().graph[edge].get();
	}

	ostringstream str;
	str << "holder not found, source_sid = " << source_sid << ", target_sid = " << target_sid;
	throw HolderNotFound(str.str());
    }


    void
    DeviceGraph::remove_vertex(sid_t sid)
    {
	const Device* device = findDevice(sid);
	getImpl().remove_vertex(device->getImpl().getVertex());
    }


    void
    DeviceGraph::remove_vertex(Device* device)
    {
	getImpl().remove_vertex(device->getImpl().getVertex());
    }


    void
    DeviceGraph::check() const
    {
	{
	    // check uniqueness of device and holder object and sid
	    // check device and holder back reference

	    set<const Device*> devices;
	    set<const Holder*> holders;
	    set<sid_t> sids;

	    for (Impl::vertex_descriptor vertex : getImpl().vertices())
	    {
		// check uniqueness of device object

		const Device* device = getImpl().graph[vertex].get();
		if (!devices.insert(device).second)
		    throw logic_error("device object not unique within graph");

		// check uniqueness of device sid

		sid_t sid = device->getSid();
		if (!sids.insert(sid).second)
		    throw logic_error("sid not unique within graph");

		// check device back reference

		if (device->getImpl().getDeviceGraph() != this)
		    throw logic_error("wrong graph in back references");

		if (device->getImpl().getVertex() != vertex)
		    throw logic_error("wrong vertex in back references");
	    }

	    for (Impl::edge_descriptor edge : getImpl().edges())
	    {
		// check uniqueness of holder object

		const Holder* holder = getImpl().graph[edge].get();
		if (!holders.insert(holder).second)
		    throw logic_error("holder object not unique within graph");

		// check holder back reference

		if (holder->getImpl().getDeviceGraph() != this)
		    throw logic_error("wrong graph in back references");

		if (holder->getImpl().getEdge() != edge)
		    throw logic_error("wrong edge in back references");
	    }
	}

	{
	    // look for cycles

	    Haha<Impl::graph_t> haha(getImpl().graph);

	    bool has_cycle = false;

	    cycle_detector vis(has_cycle);
	    boost::depth_first_search(getImpl().graph, visitor(vis).vertex_index_map(haha.get()));

	    if (has_cycle)
		cerr << "graph has a cycle" << endl;
	}

	{
	    for (Impl::vertex_descriptor v : getImpl().vertices())
	    {
		const Device* device = getImpl().graph[v].get();
		device->check();
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
	dest.getImpl().graph.clear();

	Haha<Impl::graph_t> haha(getImpl().graph);

	CloneCopier copier(*this, dest);

	boost::copy_graph(getImpl().graph, dest.getImpl().graph,
			  vertex_index_map(haha.get()).vertex_copy(copier).edge_copy(copier));
    }


    void
    DeviceGraph::print_graph() const
    {
	getImpl().print_graph();
    }


    void
    DeviceGraph::write_graphviz(const string& filename) const
    {
	getImpl().write_graphviz(filename);
    }

}
