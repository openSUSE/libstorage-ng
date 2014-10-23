

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

	for (const string& mount_point : mount_points)
	{
	    actions.push_back(new Action::RemoveFstab(sid, mount_point));
	    actions.push_back(new Action::Umount(sid, mount_point));
	}

	action_graph.add_chain(actions);
    }


    void
    Ext4::add_create_actions(ActionGraph& action_graph) const
    {
	Action::Format* format = new Action::Format(sid);
	format->first = true;
	format->last = false;
	ActionGraph::vertex_descriptor v1 = action_graph.add_vertex(format);

	if (!label.empty())
	{
	    ActionGraph::vertex_descriptor tmp = action_graph.add_vertex(new Action::SetLabel(sid));
	    action_graph.add_edge(v1, tmp);
	    v1 = tmp;
	}

	if (!mount_points.empty())
	{
	    ActionGraph::vertex_descriptor v2 = action_graph.add_vertex(new Action::Nop(sid, false, true));

	    for (const string& mount_point : mount_points)
	    {
		ActionGraph::vertex_descriptor t1 = action_graph.add_vertex(new Action::Mount(sid, mount_point));
		ActionGraph::vertex_descriptor t2 = action_graph.add_vertex(new Action::AddFstab(sid, mount_point));

		action_graph.add_edge(v1, t1);
		action_graph.add_edge(t1, t2);
		action_graph.add_edge(t2, v2);
	    }
	}
    }


    void
    Swap::add_create_actions(ActionGraph& action_graph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Format(sid));
	actions.push_back(new Action::Mount(sid, "swap"));
	actions.push_back(new Action::AddFstab(sid, "swap"));

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
