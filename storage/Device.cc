

#include <iostream>

#include "Device.h"
#include "DeviceGraph.h"
#include "Action.h"


using namespace std;


namespace storage
{

    sid_t sid = 42;


    Device::Device()
	: sid(storage::sid++)
    {
    }


    Device::~Device()
    {
    }


    void
    Device::check() const
    {
    }


    void
    Device::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(sid));

	action_graph.add_chain(actions);
    }


    void
    Device::add_delete_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(sid));

	action_graph.add_chain(actions);
    }


    BlkDevice::BlkDevice(const string& name)
	: name(name)
    {
    }


    void
    BlkDevice::check() const
    {
	if (name.empty())
	    cerr << "blk device has no name" << endl;
    }


    /*
    vector<Partition*>
    Disk::getPartitions(const DeviceGraph& device_graph)
    {
	vector<Partition*> ret;

	DeviceGraph::vertex_descriptor v = device_graph.find_vertex(sid);
	for (DeviceGraph::vertex_descriptor tmp : boost::make_iterator_range(adjacent_vertices(v, device_graph.graph)))
	{
	    Partition* partition = dynamic_cast<Partition*>(device_graph.graph[tmp].get());
	    if (partition)
		ret.push_back(partition);
	}

	return ret;
    }
    */


    void
    Partition::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(sid));
	actions.push_back(new Action::SetType(sid));

	action_graph.add_chain(actions);
    }


    void
    Encryption::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::FormatEncryption(sid));
	actions.push_back(new Action::OpenEncryption(sid));

	action_graph.add_chain(actions);
    }


    void
    Filesystem::add_delete_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::RemoveFstab(sid));
	actions.push_back(new Action::Umount(sid));

	action_graph.add_chain(actions);
    }


    void
    Ext4::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Format(sid));

	if (!label.empty())
	    actions.push_back(new Action::SetLabel(sid));

	// TODO When we support multiple mount points we have to create one
	// Mount action per mount point.  Since the Mount actions get
	// dependencies later they must either be ordered already here or not
	// depend on each other.

	if (!mount_point.empty())
	{
	    actions.push_back(new Action::Mount(sid, mount_point));
	    actions.push_back(new Action::AddFstab(sid));
	}

	action_graph.add_chain(actions);
    }


    void
    Swap::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Format(sid));
	actions.push_back(new Action::Mount(sid, "swap"));
	actions.push_back(new Action::AddFstab(sid));

	action_graph.add_chain(actions);
    }


    void
    LvmVg::check() const
    {
	if (name.empty())
	    cerr << "volume groups has no name" << endl;

	// TODO check that a volume group has enough physical volumes
    }


    void
    LvmLv::check() const
    {
	if (name.empty())
	    cerr << "logical volume has no name" << endl;
    }

}
