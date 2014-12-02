

#include "storage/Devices/SwapImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Swap::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Swap::Impl::save(xmlNode* node) const
    {
	Filesystem::Impl::save(node);
    }


    void
    Swap::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Format(getSid()));
	actions.push_back(new Action::Mount(getSid(), "swap"));
	actions.push_back(new Action::AddFstab(getSid(), "swap"));

	action_graph.add_chain(actions);
    }

}
