

#include "storage/Devices/DiskImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    void
    Disk::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    void
    Disk::Impl::add_create_actions(ActionGraph& action_graph) const
    {
	throw runtime_error("cannot create disk");
    }


    void
    Disk::Impl::add_delete_actions(ActionGraph& action_graph) const
    {
	throw runtime_error("cannot delete disk");
    }

}
