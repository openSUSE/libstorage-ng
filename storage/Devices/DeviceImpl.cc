

#include "storage/Devices/DeviceImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    sid_t Device::Impl::global_sid = 42;	// just a random number ;)


    Device::Impl::Impl(DeviceGraph& device_graph)
	: sid(global_sid++), device_graph(device_graph)
    {
    }


    // TODO not nice that all members must be initialized individual

    Device::Impl::Impl(DeviceGraph& device_graph, const Impl& impl)
	: sid(impl.sid), device_graph(device_graph)
    {
    }


    void
    Device::Impl::setVertex(DeviceGraph::Impl::vertex_descriptor vertex)
    {
	Impl::vertex = vertex;

	const Device* d = device_graph.getImpl().graph[vertex].get();
	const Impl& i = d->getImpl();
	if (&i != this)
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
