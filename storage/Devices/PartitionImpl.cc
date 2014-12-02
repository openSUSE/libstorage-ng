

#include "storage/Devices/PartitionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Partition::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    Partition::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    void
    Partition::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(getSid()));
	actions.push_back(new Action::SetType(getSid()));

	action_graph.add_chain(actions);
    }

}
