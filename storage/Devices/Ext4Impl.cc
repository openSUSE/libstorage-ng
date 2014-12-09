

#include "storage/Devices/Ext4Impl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Ext4::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Ext4::Impl::save(xmlNode* node) const
    {
	Filesystem::Impl::save(node);
    }


    void
    Ext4::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	Action::Format* format = new Action::Format(get_sid());
	format->first = true;
	format->last = false;
	Actiongraph::vertex_descriptor v1 = actiongraph.add_vertex(format);

	if (!label.empty())
	{
	    Actiongraph::vertex_descriptor tmp = actiongraph.add_vertex(new Action::SetLabel(get_sid()));
	    actiongraph.add_edge(v1, tmp);
	    v1 = tmp;
	}

	if (!mountpoints.empty())
	{
	    Actiongraph::vertex_descriptor v2 = actiongraph.add_vertex(new Action::Nop(get_sid(), false, true));

	    for (const string& mountpoint : mountpoints)
	    {
		Actiongraph::vertex_descriptor t1 = actiongraph.add_vertex(new Action::Mount(get_sid(), mountpoint));
		Actiongraph::vertex_descriptor t2 = actiongraph.add_vertex(new Action::AddFstab(get_sid(), mountpoint));

		actiongraph.add_edge(v1, t1);
		actiongraph.add_edge(t1, t2);
		actiongraph.add_edge(t2, v2);
	    }
	}
    }

}
