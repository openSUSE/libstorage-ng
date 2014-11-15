

#include "storage/Devices/PartitionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    void
    Partition::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(sid));
	actions.push_back(new Action::SetType(sid));

	action_graph.add_chain(actions);
    }

}
