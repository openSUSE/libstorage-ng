

#include "storage/Devices/Ext4Impl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    void
    Ext4::Impl::save(xmlNode* node) const
    {
	Filesystem::Impl::save(node);
    }


    void
    Ext4::Impl::add_create_actions(ActionGraph& action_graph) const
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

}
