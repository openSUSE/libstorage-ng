

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


    Device::Device(DeviceGraph& device_graph, Impl* impl)
	: Device(impl)
    {
	getImpl().setVertex(boost::add_vertex(shared_ptr<Device>(this), device_graph.graph));
    }


    Device::~Device()
    {
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
	return getImpl().sid;
    }


    void
    Device::check() const
    {
    }


    vector<const Device*>
    Device::getChildren() const
    {
	const DeviceGraph& device_graph = getImpl().getDeviceGraph();
	DeviceGraph::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph.getDevices<Device>(device_graph.children(vertex));
    }


    vector<const Device*>
    Device::getParents() const
    {
	const DeviceGraph& device_graph = getImpl().getDeviceGraph();
	DeviceGraph::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph.getDevices<Device>(device_graph.parents(vertex));
    }



    vector<const Device*>
    Device::getSiblings(bool itself) const
    {
	const DeviceGraph& device_graph = getImpl().getDeviceGraph();
	DeviceGraph::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph.getDevices<Device>(device_graph.siblings(vertex, itself));
    }


    vector<const Device*>
    Device::getDescendants(bool itself) const
    {
	const DeviceGraph& device_graph = getImpl().getDeviceGraph();
	DeviceGraph::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph.getDevices<Device>(device_graph.descendants(vertex, itself));
    }


    vector<const Device*>
    Device::getAncestors(bool itself) const
    {
	const DeviceGraph& device_graph = getImpl().getDeviceGraph();
	DeviceGraph::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph.getDevices<Device>(device_graph.ancestors(vertex, itself));
    }


    vector<const Device*>
    Device::getLeafs(bool itself) const
    {
	const DeviceGraph& device_graph = getImpl().getDeviceGraph();
	DeviceGraph::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph.getDevices<Device>(device_graph.leafs(vertex, itself));
    }


    vector<const Device*>
    Device::getRoots(bool itself) const
    {
	const DeviceGraph& device_graph = getImpl().getDeviceGraph();
	DeviceGraph::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph.getDevices<Device>(device_graph.roots(vertex, itself));
    }

}
