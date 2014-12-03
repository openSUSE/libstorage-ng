

#include "storage/Holders/HolderImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Holder::Impl::Impl()
	: device_graph(nullptr)
    {
    }


    Holder::Impl::Impl(const xmlNode* node)
	: device_graph(nullptr)
    {
    }


    void
    Holder::Impl::setDeviceGraphAndEdge(DeviceGraph* device_graph,
					DeviceGraph::Impl::edge_descriptor edge)
    {
	Impl::device_graph = device_graph;
	Impl::edge = edge;

	const Holder* holder = device_graph->getImpl().graph[edge].get();
	if (&holder->getImpl() != this)
	    throw runtime_error("wrong edge for back references");
    }


    sid_t
    Holder::Impl::getSourceSid() const
    {
	const DeviceGraph::Impl::graph_t& graph = device_graph->getImpl().graph;
	return graph[source(edge, graph)]->getSid();
    }


    sid_t
    Holder::Impl::getTargetSid() const
    {
	const DeviceGraph::Impl::graph_t& graph = device_graph->getImpl().graph;
	return graph[target(edge, graph)]->getSid();
    }


    void
    Holder::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "source-sid", getSourceSid());
	setChildValue(node, "target-sid", getTargetSid());
    }

}
