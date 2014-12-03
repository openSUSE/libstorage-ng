#ifndef HOLDER_IMPL_H
#define HOLDER_IMPL_H


#include <libxml/tree.h>

#include "storage/Holders/Holder.h"
#include "storage/DeviceGraphImpl.h"


namespace storage
{

    // abstract class

    class Holder::Impl
    {
    public:

	virtual ~Impl() {}

	virtual Impl* clone() const = 0;

	virtual void save(xmlNode* node) const = 0;

	void setDeviceGraphAndEdge(DeviceGraph* device_graph,
				   DeviceGraph::Impl::edge_descriptor edge);

	DeviceGraph* getDeviceGraph() { return device_graph; }
	const DeviceGraph* getDeviceGraph() const { return device_graph; }

	DeviceGraph::Impl::edge_descriptor getEdge() const { return edge; }

	sid_t getSourceSid() const;
	sid_t getTargetSid() const;

    protected:

	Impl();

	Impl(const xmlNode* node);

    private:

	DeviceGraph* device_graph;
	DeviceGraph::Impl::edge_descriptor edge;

    };

}

#endif
