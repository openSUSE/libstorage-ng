

#include "storage/Devices/GptImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Gpt::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node)
    {
    }


    void
    Gpt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);
    }


    void
    Gpt::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(getSid()));

	action_graph.add_chain(actions);
    }


    void
    Gpt::Impl::add_delete_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Nop(getSid()));

	action_graph.add_chain(actions);
    }

}
