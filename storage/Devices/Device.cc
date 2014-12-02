

#include "storage/Devices/DeviceImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Device::Device(Impl* impl)
	: impl(impl)
    {
	if (!impl)
	    throw runtime_error("impl is nullptr");
    }


    Device::~Device()
    {
    }


    void
    Device::addToDeviceGraph(DeviceGraph* device_graph)
    {
	DeviceGraph::Impl::vertex_descriptor vertex =
	    boost::add_vertex(shared_ptr<Device>(this), device_graph->getImpl().graph);

	getImpl().setDeviceGraphAndVertex(device_graph, vertex);
    }


    void
    Device::save(xmlNode* node) const
    {
	getImpl().save(node);
    }


    Device::Impl&
    Device::getImpl()
    {
	return *impl;
    }


    const Device::Impl&
    Device::getImpl() const
    {
	return *impl;
    }


    sid_t
    Device::getSid() const
    {
	return getImpl().getSid();
    }


    void
    Device::check() const
    {
    }


    size_t
    Device::numChildren() const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().num_children(vertex);
    }


    size_t
    Device::numParents() const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().num_parents(vertex);
    }


    vector<const Device*>
    Device::getChildren() const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Device>(device_graph->getImpl().children(vertex));
    }


    vector<const Device*>
    Device::getParents() const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Device>(device_graph->getImpl().parents(vertex));
    }


    vector<const Device*>
    Device::getSiblings(bool itself) const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Device>(device_graph->getImpl().siblings(vertex, itself));
    }


    vector<const Device*>
    Device::getDescendants(bool itself) const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Device>(device_graph->getImpl().descendants(vertex, itself));
    }


    vector<const Device*>
    Device::getAncestors(bool itself) const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Device>(device_graph->getImpl().ancestors(vertex, itself));
    }


    vector<const Device*>
    Device::getLeafs(bool itself) const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Device>(device_graph->getImpl().leafs(vertex, itself));
    }


    vector<const Device*>
    Device::getRoots(bool itself) const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Device>(device_graph->getImpl().roots(vertex, itself));
    }

}
