

#include "storage/Devices/DeviceImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    sid_t Device::Impl::global_sid = 42;	// just a random number ;)


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
