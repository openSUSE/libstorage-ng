

#include <boost/graph/copy.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>

#include "storage/Devicegraph.h"
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
#include "storage/Utils/AppUtil.h"


namespace storage
{

    Devicegraph::Devicegraph()
	: impl(new Impl())
    {
    }


    Devicegraph::~Devicegraph()
    {
    }


    bool
    Devicegraph::operator==(const Devicegraph& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Devicegraph::operator!=(const Devicegraph& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    void
    Devicegraph::load(const string& filename)
    {
	get_impl().load(this, filename);
    }


    void
    Devicegraph::save(const string& filename) const
    {
	get_impl().save(filename);
    }


    bool
    Devicegraph::empty() const
    {
	return get_impl().empty();
    }


    size_t
    Devicegraph::num_devices() const
    {
	return get_impl().num_devices();
    }


    size_t
    Devicegraph::num_holders() const
    {
	return get_impl().num_holders();
    }


    class CloneCopier
    {

    public:

	CloneCopier(const Devicegraph& g_in, Devicegraph& g_out)
	    : g_in(g_in), g_out(g_out) {}

	void operator()(const Devicegraph::Impl::vertex_descriptor& v_in,
			Devicegraph::Impl::vertex_descriptor& v_out)
	{
	    g_out.get_impl().graph[v_out].reset(g_in.get_impl().graph[v_in]->clone());

	    Device* d_out = g_out.get_impl().graph[v_out].get();
	    d_out->get_impl().set_devicegraph_and_vertex(&g_out, v_out);
	}

	void operator()(const Devicegraph::Impl::edge_descriptor& e_in,
			Devicegraph::Impl::edge_descriptor& e_out)
	{
	    g_out.get_impl().graph[e_out].reset(g_in.get_impl().graph[e_in]->clone());

	    Holder* h_out = g_out.get_impl().graph[e_out].get();
	    h_out->get_impl().set_devicegraph_and_edge(&g_out, e_out);
	}

    private:

	const Devicegraph& g_in;
	Devicegraph& g_out;

    };


    Device*
    Devicegraph::find_device(sid_t sid)
    {
	Impl::vertex_descriptor v = get_impl().find_vertex(sid);

	return get_impl().graph[v].get();
    }


    const Device*
    Devicegraph::find_device(sid_t sid) const
    {
	Impl::vertex_descriptor vertex = get_impl().find_vertex(sid);
	return get_impl().graph[vertex].get();
    }


    bool
    Devicegraph::vertex_exists(sid_t sid) const
    {
	try
	{
	    get_impl().find_vertex(sid);
	    return true;
	}
	catch (const runtime_error& e)
	{
	    return false;
	}
    }


    Holder*
    Devicegraph::find_holder(sid_t source_sid, sid_t target_sid)
    {
	Impl::edge_descriptor edge = get_impl().find_edge(source_sid, target_sid);
	return get_impl().graph[edge].get();
    }


    const Holder*
    Devicegraph::find_holder(sid_t source_sid, sid_t target_sid) const
    {
	Impl::edge_descriptor edge = get_impl().find_edge(source_sid, target_sid);
	return get_impl().graph[edge].get();
    }


    void
    Devicegraph::clear()
    {
	get_impl().clear();
    }


    void
    Devicegraph::remove_vertex(sid_t sid)
    {
	const Device* device = find_device(sid);
	get_impl().remove_vertex(device->get_impl().get_vertex());
    }


    void
    Devicegraph::remove_vertex(Device* device)
    {
	get_impl().remove_vertex(device->get_impl().get_vertex());
    }


    void
    Devicegraph::check() const
    {
	{
	    // check uniqueness of device and holder object and sid
	    // check device and holder back reference

	    set<const Device*> devices;
	    set<const Holder*> holders;
	    set<sid_t> sids;

	    for (Impl::vertex_descriptor vertex : get_impl().vertices())
	    {
		// check uniqueness of device object

		const Device* device = get_impl().graph[vertex].get();
		if (!devices.insert(device).second)
		    throw logic_error("device object not unique within graph");

		// check uniqueness of device sid

		sid_t sid = device->get_sid();
		if (!sids.insert(sid).second)
		    throw logic_error("sid not unique within graph");

		// check device back reference

		if (device->get_impl().get_devicegraph() != this)
		    throw logic_error("wrong graph in back references");

		if (device->get_impl().get_vertex() != vertex)
		    throw logic_error("wrong vertex in back references");
	    }

	    for (Impl::edge_descriptor edge : get_impl().edges())
	    {
		// check uniqueness of holder object

		const Holder* holder = get_impl().graph[edge].get();
		if (!holders.insert(holder).second)
		    throw logic_error("holder object not unique within graph");

		// check holder back reference

		if (holder->get_impl().get_devicegraph() != this)
		    throw logic_error("wrong graph in back references");

		if (holder->get_impl().get_edge() != edge)
		    throw logic_error("wrong edge in back references");
	    }
	}

	{
	    // look for cycles

	    Haha<Impl::graph_t> haha(get_impl().graph);

	    bool has_cycle = false;

	    cycle_detector vis(has_cycle);
	    boost::depth_first_search(get_impl().graph, visitor(vis).vertex_index_map(haha.get()));

	    if (has_cycle)
		cerr << "graph has a cycle" << endl;
	}

	{
	    for (Impl::vertex_descriptor v : get_impl().vertices())
	    {
		const Device* device = get_impl().graph[v].get();
		device->check();
	    }
	}

	// TODO check parallel edges if not using boost::setS

	// TODO check that out-edges are consistent, e.g. of same type, only one per Subdevice
	// TODO check that in-edges are consistent, e.g. of same type, exactly one for Partition
	// in general subcheck for each device
    }


    void
    Devicegraph::copy(Devicegraph& dest) const
    {
	dest.get_impl().graph.clear();

	Haha<Impl::graph_t> haha(get_impl().graph);

	CloneCopier copier(*this, dest);

	boost::copy_graph(get_impl().graph, dest.get_impl().graph,
			  vertex_index_map(haha.get()).vertex_copy(copier).edge_copy(copier));
    }


    std::ostream&
    operator<<(std::ostream& out, const Devicegraph& devicegraph)
    {
	devicegraph.get_impl().print(out);
	return out;
    }


    void
    Devicegraph::write_graphviz(const string& filename) const
    {
	get_impl().write_graphviz(filename);
    }

}
