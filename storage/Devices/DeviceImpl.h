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

	virtual const char* getClassName() const = 0;

	virtual Impl* clone(DeviceGraph& device_graph) const = 0;

	virtual void save(xmlNode* node) const = 0;

	sid_t getSid() const { return sid; }

	const sid_t sid;

	DeviceGraph& getDeviceGraph() { return device_graph; }
	const DeviceGraph& getDeviceGraph() const { return device_graph; }

	DeviceGraph::Impl::vertex_descriptor getVertex() const { return vertex; }
	void setVertex(DeviceGraph::Impl::vertex_descriptor vertex);

	virtual void add_create_actions(ActionGraph& action_graph) const;
	virtual void add_delete_actions(ActionGraph& action_graph) const;

    protected:

	Impl(DeviceGraph& device_graph);
	Impl(DeviceGraph& device_graph, const Impl& impl);

    private:

	static sid_t global_sid;

	DeviceGraph& device_graph;
	DeviceGraph::Impl::vertex_descriptor vertex;

    };

}

#endif
