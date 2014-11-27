

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


namespace storage
{

    DeviceGraph::DeviceGraph()
	: impl(new Impl())
    {
    }


    DeviceGraph::~DeviceGraph()
    {
    }


    DeviceGraph::Impl&
    DeviceGraph::getImpl()
    {
	return *impl;
    }


    const DeviceGraph::Impl&
    DeviceGraph::getImpl() const
    {
	return *impl;
    }


    bool
    DeviceGraph::empty() const
    {
	return numVertices() == 0;
    }


    size_t
    DeviceGraph::numVertices() const
    {
	return boost::num_vertices(getImpl().graph);
    }


    size_t
    DeviceGraph::numEdges() const
    {
	return boost::num_edges(getImpl().graph);
    }


    class CloneCopier
    {

    public:

	CloneCopier(const DeviceGraph& g_in, DeviceGraph& g_out)
	    : g_in(g_in), g_out(g_out) {}

	template<class Type>
	void operator()(const Type& v_in, Type& v_out)
	{
	    g_out.getImpl().graph[v_out].reset(g_in.getImpl().graph[v_in]->clone(g_out));

	    Device* d_out = g_out.getImpl().graph[v_out].get();
	    d_out->getImpl().setVertex(v_out);
	}

    private:

	const DeviceGraph& g_in;
	DeviceGraph& g_out;

    };


    Device*
    DeviceGraph::find_device(sid_t sid)
    {
	Impl::vertex_descriptor v = getImpl().find_vertex(sid);

	return getImpl().graph[v].get();
    }


    const Device*
    DeviceGraph::find_device(sid_t sid) const
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
    DeviceGraph::find_holder(sid_t source_sid, sid_t target_sid)
    {
	for (Impl::edge_descriptor edge : getImpl().edges())
	{
	    if (getImpl().graph[source(edge, getImpl().graph)]->getSid() == source_sid &&
		getImpl().graph[target(edge, getImpl().graph)]->getSid() == target_sid)
		return getImpl().graph[edge].get();
	}

	ostringstream str;
	str << "holder not found, source_sid = " << source_sid << ", target_sid = " << target_sid;
	throw runtime_error(str.str());
    }


    const Holder*
    DeviceGraph::find_holder(sid_t source_sid, sid_t target_sid) const
    {
	for (Impl::edge_descriptor edge : getImpl().edges())
	{
	    if (getImpl().graph[source(edge, getImpl().graph)]->getSid() == source_sid &&
		getImpl().graph[target(edge, getImpl().graph)]->getSid() == target_sid)
		return getImpl().graph[edge].get();
	}

	ostringstream str;
	str << "holder not found, source_sid = " << source_sid << ", target_sid = " << target_sid;
	throw runtime_error(str.str());
    }


    void
    DeviceGraph::remove_vertex(sid_t sid)
    {
	const Device* device = find_device(sid);
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
	    // check uniqueness of device object and sid
	    // check device back reference

	    set<const Device*> devices;
	    set<sid_t> sids;

	    for (Impl::vertex_descriptor vertex : getImpl().vertices())
	    {
		const Device* device = getImpl().graph[vertex].get();
		if (!devices.insert(device).second)
		    throw logic_error("device object not unique within graph");

		sid_t sid = device->getSid();
		if (!sids.insert(sid).second)
		    throw logic_error("sid not unique within graph");

		if (&device->getImpl().getDeviceGraph() != this)
		    throw logic_error("wrong graph in back references");

		if (device->getImpl().getVertex() != vertex)
		    throw logic_error("wrong vertex in back references");
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
			  vertex_index_map(haha.get()).vertex_copy(copier));
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
