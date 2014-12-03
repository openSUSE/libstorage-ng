

#include "storage/Devices/DeviceImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    using namespace std;


    sid_t Device::Impl::global_sid = 42;	// just a random number ;)


    Device::Impl::Impl()
	: sid(global_sid++), device_graph(nullptr)
    {
    }


    Device::Impl::Impl(const xmlNode* node)
	: sid(0), device_graph(nullptr)
    {
	if (!getChildValue(node, "sid", sid))
	    throw runtime_error("no sid");
    }


    void
    Device::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "sid", sid);
    }


    void
    Device::Impl::setDeviceGraphAndVertex(DeviceGraph* device_graph,
					  DeviceGraph::Impl::vertex_descriptor vertex)
    {
	Impl::device_graph = device_graph;
	Impl::vertex = vertex;

	const Device* device = device_graph->getImpl().graph[vertex].get();
	if (&device->getImpl() != this)
	    throw runtime_error("wrong vertex for back references");
    }


    void
    Device::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(sid));

	action_graph.add_chain(actions);
    }


    void
    Device::Impl::add_delete_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(sid));

	action_graph.add_chain(actions);
    }

}
