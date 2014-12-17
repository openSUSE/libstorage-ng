

#include "storage/Devices/DeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    using namespace std;


    sid_t Device::Impl::global_sid = 42;	// just a random number ;)


    Device::Impl::Impl()
	: sid(global_sid++), devicegraph(nullptr)
    {
    }


    Device::Impl::Impl(const xmlNode* node)
	: sid(0), devicegraph(nullptr)
    {
	if (!getChildValue(node, "sid", sid))
	    throw runtime_error("no sid");
    }


    void
    Device::Impl::probe(SystemInfo& systeminfo)
    {
    }


    void
    Device::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "sid", sid);
    }


    void
    Device::Impl::set_devicegraph_and_vertex(Devicegraph* devicegraph,
					  Devicegraph::Impl::vertex_descriptor vertex)
    {
	Impl::devicegraph = devicegraph;
	Impl::vertex = vertex;

	const Device* device = devicegraph->get_impl().graph[vertex].get();
	if (&device->get_impl() != this)
	    throw runtime_error("wrong vertex for back references");
    }


    void
    Device::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(sid));

	actiongraph.add_chain(actions);
    }


    void
    Device::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(sid));

	actiongraph.add_chain(actions);
    }

}
