

#include "storage/Holders/Holder.h"
#include "storage/DeviceGraph.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    Holder::Holder(DeviceGraph& device_graph, Device* source, Device* target)
    {
	if (&source->getImpl().getDeviceGraph() != &device_graph)
	    throw runtime_error("wrong graph in source");
	
	if (&target->getImpl().getDeviceGraph() != &device_graph)
	    throw runtime_error("wrong graph in target");

	DeviceGraph::vertex_descriptor source_vertex = source->getImpl().getVertex();
	DeviceGraph::vertex_descriptor target_vertex = target->getImpl().getVertex();

	pair<DeviceGraph::edge_descriptor, bool> tmp =
	    boost::add_edge(source_vertex, target_vertex, shared_ptr<Holder>(this), device_graph.graph);

	if (!tmp.second)
	    throw runtime_error("holder already exists");
    }

}
