

#include "storage/Devices/SwapImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    void
    Swap::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Format(sid));
	actions.push_back(new Action::Mount(sid, "swap"));
	actions.push_back(new Action::AddFstab(sid, "swap"));

	action_graph.add_chain(actions);
    }

}
