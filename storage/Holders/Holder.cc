

#include "storage/Holders/HolderImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/DeviceGraph.h"


namespace storage
{

    Holder::Holder(Impl* impl)
	: impl(impl)
    {
	if (!impl)
	    throw runtime_error("impl is nullptr");
    }


    Holder::~Holder()
    {
    }


    Holder::Impl&
    Holder::getImpl()
    {
	return *impl;
    }


    const Holder::Impl&
    Holder::getImpl() const
    {
	return *impl;
    }


    sid_t
    Holder::getSourceSid() const
    {
	return getImpl().getSourceSid();
    }


    sid_t
    Holder::getTargetSid() const
    {
	return getImpl().getTargetSid();
    }


    void
    Holder::addToDeviceGraph(DeviceGraph* device_graph, const Device* source,
			     const Device* target)
    {
	if (source->getImpl().getDeviceGraph() != device_graph)
	    throw runtime_error("wrong graph in source");

	if (target->getImpl().getDeviceGraph() != device_graph)
	    throw runtime_error("wrong graph in target");

	DeviceGraph::Impl::vertex_descriptor source_vertex = source->getImpl().getVertex();
	DeviceGraph::Impl::vertex_descriptor target_vertex = target->getImpl().getVertex();

	pair<DeviceGraph::Impl::edge_descriptor, bool> tmp =
	    boost::add_edge(source_vertex, target_vertex, shared_ptr<Holder>(this),
			    device_graph->getImpl().graph);

	if (!tmp.second)
	    throw runtime_error("holder already exists");

	getImpl().setDeviceGraphAndEdge(device_graph, tmp.first);
    }


    void
    Holder::save(xmlNode* node) const
    {
	getImpl().save(node);
    }

}
