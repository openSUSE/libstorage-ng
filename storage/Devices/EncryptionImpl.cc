

#include "storage/Devices/EncryptionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    void
    Encryption::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    void
    Encryption::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::FormatEncryption(sid));
	actions.push_back(new Action::OpenEncryption(sid));

	action_graph.add_chain(actions);
    }

}
