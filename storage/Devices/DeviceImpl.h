#ifndef DEVICE_IMPL_H
#define DEVICE_IMPL_H


#include <libxml/tree.h>

#include "storage/Devices/Device.h"
#include "storage/DeviceGraph.h"
#include "storage/DeviceGraphImpl.h"


namespace storage
{

    using namespace std;


    class ActionGraph;


    // abstract class

    class Device::Impl
    {
    public:

	virtual ~Impl() {}

	virtual Impl* clone() const = 0;

	virtual void save(xmlNode* node) const = 0;

	sid_t getSid() const { return sid; }

	void setDeviceGraphAndVertex(DeviceGraph* device_graph,
				     DeviceGraph::Impl::vertex_descriptor vertex);

	DeviceGraph* getDeviceGraph() { return device_graph; }
	const DeviceGraph* getDeviceGraph() const { return device_graph; }

	DeviceGraph::Impl::vertex_descriptor getVertex() const { return vertex; }

	virtual void add_create_actions(ActionGraph& action_graph) const;
	virtual void add_delete_actions(ActionGraph& action_graph) const;

    protected:

	Impl();

	Impl(const xmlNode* node);

    private:

	static sid_t global_sid;

	sid_t sid;

	DeviceGraph* device_graph;
	DeviceGraph::Impl::vertex_descriptor vertex;

    };

}

#endif
