

#include "storage/Holders/Holder.h"
#include "storage/DeviceGraph.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    Holder::Holder(DeviceGraph* device_graph, const Device* source, const Device* target)
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
    }


    void
    Holder::save(xmlNode* node) const
    {
	// TODO
    }

}
