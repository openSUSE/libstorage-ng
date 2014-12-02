

#include "storage/Devices/FilesystemImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Filesystem::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
    }


    void
    Filesystem::Impl::save(xmlNode* node) const
    {
    }


    void
    Filesystem::Impl::add_delete_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	for (const string& mount_point : mount_points)
	{
	    actions.push_back(new Action::RemoveFstab(getSid(), mount_point));
	    actions.push_back(new Action::Umount(getSid(), mount_point));
	}

	action_graph.add_chain(actions);
    }

}
