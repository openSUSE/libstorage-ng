

#include <boost/graph/copy.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>

#include "storage/Devicegraph.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Filesystem.h"
#include "storage/Holders/HolderImpl.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Logger.h"


namespace storage
{


    DeviceNotFound::DeviceNotFound(sid_t sid)
	: Exception(sformat("device not found, sid:%d", sid))
    {
    }


    DeviceNotFound::DeviceNotFound(const string& name)
	: Exception(sformat("device not found, name:%s", name.c_str()))
    {
    }


    HolderNotFound::HolderNotFound(sid_t source_sid, sid_t target_sid)
	: Exception(sformat("holder not found, source_sid:%d, target_sid:%d", source_sid, target_sid))
    {
    }


    HolderAlreadyExists::HolderAlreadyExists(sid_t source_sid, sid_t target_sid)
	: Exception(sformat("holder already exists, source_sid:%d, target_sid:%d", source_sid, target_sid))
    {
    }


    WrongNumberOfParents::WrongNumberOfParents(size_t seen, size_t expected)
	: Exception(sformat("wrong number of parents, seen '%zu', expected '%zu'", seen, expected),
		    Silencer::is_any_active() ? DEBUG : WARNING)
    {
    }


    WrongNumberOfChildren::WrongNumberOfChildren(size_t seen, size_t expected)
	: Exception(sformat("wrong number of children, seen '%zu', expected '%zu'", seen, expected),
		    Silencer::is_any_active() ? DEBUG : WARNING)
    {
    }


    Devicegraph::Devicegraph(const Storage* storage)
	: impl(new Impl(storage))
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


    const Storage*
    Devicegraph::get_storage() const
    {
	return get_impl().get_storage();
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


    std::vector<Disk*>
    Devicegraph::get_all_disks()
    {
	return Disk::get_all(this);
    }


    std::vector<const Disk*>
    Devicegraph::get_all_disks() const
    {
	return Disk::get_all(this);
    }


    std::vector<Md*>
    Devicegraph::get_all_mds()
    {
	return Md::get_all(this);
    }


    std::vector<const Md*>
    Devicegraph::get_all_mds() const
    {
	return Md::get_all(this);
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
	Impl::vertex_descriptor vertex = get_impl().find_vertex(sid);
	return get_impl()[vertex];
    }


    const Device*
    Devicegraph::find_device(sid_t sid) const
    {
	Impl::vertex_descriptor vertex = get_impl().find_vertex(sid);
	return get_impl()[vertex];
    }


    bool
    Devicegraph::device_exists(sid_t sid) const
    {
	try
	{
	    get_impl().find_vertex(sid);
	    return true;
	}
	catch (const DeviceNotFound& e)
	{
	    ST_CAUGHT(e);
	    return false;
	}
    }


    Holder*
    Devicegraph::find_holder(sid_t source_sid, sid_t target_sid)
    {
	Impl::edge_descriptor edge = get_impl().find_edge(source_sid, target_sid);
	return get_impl()[edge];
    }


    const Holder*
    Devicegraph::find_holder(sid_t source_sid, sid_t target_sid) const
    {
	Impl::edge_descriptor edge = get_impl().find_edge(source_sid, target_sid);
	return get_impl()[edge];
    }


    void
    Devicegraph::clear()
    {
	get_impl().clear();
    }


    void
    Devicegraph::remove_device(sid_t sid)
    {
	const Device* device = find_device(sid);
	get_impl().remove_vertex(device->get_impl().get_vertex());
    }


    void
    Devicegraph::remove_device(Device* device)
    {
	get_impl().remove_vertex(device->get_impl().get_vertex());
    }


    void
    Devicegraph::remove_devices(std::vector<Device*> devices)
    {
	for (Device* device : devices)
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

		const Device* device = get_impl()[vertex];
		if (!devices.insert(device).second)
		    ST_THROW(LogicException("device object not unique within graph"));

		// check uniqueness of device sid

		sid_t sid = device->get_sid();
		if (!sids.insert(sid).second)
		    ST_THROW(LogicException("sid not unique within graph"));

		// check device back reference

		if (device->get_impl().get_devicegraph() != this)
		    ST_THROW(LogicException("wrong graph in back references"));

		if (device->get_impl().get_vertex() != vertex)
		    ST_THROW(LogicException("wrong vertex in back references"));
	    }

	    for (Impl::edge_descriptor edge : get_impl().edges())
	    {
		// check uniqueness of holder object

		const Holder* holder = get_impl()[edge];
		if (!holders.insert(holder).second)
		    ST_THROW(LogicException("holder object not unique within graph"));

		// check holder back reference

		if (holder->get_impl().get_devicegraph() != this)
		    ST_THROW(LogicException("wrong graph in back references"));

		if (holder->get_impl().get_edge() != edge)
		    ST_THROW(LogicException("wrong edge in back references"));
	    }
	}

	{
	    // look for cycles

	    VertexIndexMapGenerator<Impl::graph_t> vertex_index_map_generator(get_impl().graph);

	    bool has_cycle = false;

	    CycleDetector cycle_detector(has_cycle);
	    boost::depth_first_search(get_impl().graph, visitor(cycle_detector).
				      vertex_index_map(vertex_index_map_generator.get()));

	    if (has_cycle)
		cerr << "graph has a cycle" << endl;
	}

	{
	    for (Impl::vertex_descriptor v : get_impl().vertices())
	    {
		const Device* device = get_impl()[v];
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
	dest.get_impl().clear();

	VertexIndexMapGenerator<Impl::graph_t> vertex_index_map_generator(get_impl().graph);

	CloneCopier copier(*this, dest);

	boost::copy_graph(get_impl().graph, dest.get_impl().graph,
			  vertex_index_map(vertex_index_map_generator.get()).
			  vertex_copy(copier).edge_copy(copier));
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
