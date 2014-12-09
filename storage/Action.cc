

#include <fstream>

#include "storage/Action.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/Partition.h"


namespace storage
{

    namespace Action
    {

	string
	Nop::text(const Actiongraph& actiongraph, bool doing) const
	{
	    ostringstream str;
	    str << sid << " nop";
	    return str.str();
	}


	string
	Create::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " create " << device_rhs->get_displayname();
	    return str.str();
	}


	string
	SetType::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " set type " << device_rhs->get_displayname();
	    return str.str();
	}


	string
	Format::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " format " << device_rhs->get_displayname();
	    return str.str();
	}


	string
	SetLabel::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " set label " << device_rhs->get_displayname();
	    return str.str();
	}


	string
	FormatEncryption::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " format " << device_rhs->get_displayname();
	    return str.str();
	}


	string
	OpenEncryption::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " open " << device_rhs->get_displayname();
	    return str.str();
	}


	string
	Mount::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " mount " << device_rhs->get_displayname() << " " << mount_point;
	    return str.str();
	}


	string
	Umount::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_lhs = actiongraph.lhs.find_device(sid);

	    ostringstream str;
	    str << sid << " umount " << device_lhs->get_displayname() << " " << mount_point;
	    return str.str();
	}


	string
	AddFstab::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " add fstab entry " << device_rhs->get_displayname() << " " << mount_point;
	    return str.str();
	}


	string
	RemoveFstab::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_lhs = actiongraph.lhs.find_device(sid);

	    ostringstream str;
	    str << sid << " remove fstab entry " << device_lhs->get_displayname() << " " << mount_point;
	    return str.str();
	}


	string
	Modify::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_lhs = actiongraph.lhs.find_device(sid);
	    const Device* device_rhs = actiongraph.rhs.find_device(sid);

	    ostringstream str;
	    str << sid << " rename " << device_lhs->get_displayname() << " -> "
		<< device_rhs->get_displayname();
	    return str.str();
	}


	string
	Delete::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_lhs = actiongraph.lhs.find_device(sid);

	    ostringstream str;
	    str << sid << " delete " << device_lhs->get_displayname();
	    return str.str();
	}


	void
	Create::add_dependencies(Actiongraph::vertex_descriptor v, Actiongraph& actiongraph) const
	{
	    sid_t sid = actiongraph.graph[v]->sid;

	    Devicegraph::Impl::vertex_descriptor v_in_rhs = actiongraph.rhs.get_impl().find_vertex(sid);

	    // iterate parents
	    Devicegraph::Impl::graph_t::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_rhs, actiongraph.rhs.get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t parent_sid = actiongraph.rhs.get_impl().graph[*vi]->get_sid();

		if (!actiongraph.lhs.vertex_exists(parent_sid))
		{
		    // parents must be created beforehand if not existed

		    Actiongraph::vertex_descriptor tmp = actiongraph.huhu(parent_sid, false, true).front();
		    actiongraph.add_edge(tmp, v);
		}
		else
		{
		    // children of parents must be deleted beforehand

		    Devicegraph::Impl::vertex_descriptor q = actiongraph.lhs.get_impl().find_vertex(parent_sid);

		    Devicegraph::Impl::graph_t::adjacency_iterator vi2, vi2_end;
		    for (boost::tie(vi2, vi2_end) = adjacent_vertices(q, actiongraph.lhs.get_impl().graph); vi2 != vi2_end; ++vi2)
		    {
			sid_t child_sid = actiongraph.lhs.get_impl().graph[*vi2]->get_sid();

			Actiongraph::vertex_descriptor tmp = actiongraph.huhu(child_sid, false, true).front();
			actiongraph.add_edge(tmp, v);
		    }
		}
	    }

	    // create order of partitions
	    if (dynamic_cast<const Partition*>(actiongraph.rhs.get_impl().graph[v_in_rhs].get()))
	    {
		vector<Devicegraph::Impl::vertex_descriptor> siblings = actiongraph.rhs.get_impl().siblings(v_in_rhs, false);

		vector<Actiongraph::vertex_descriptor> w;

		for (Devicegraph::Impl::vertex_descriptor q : siblings)
		{
		    sid_t s_sid = actiongraph.rhs.get_impl().graph[q]->get_sid();

		    for (Actiongraph::vertex_descriptor tmp : actiongraph.vertices())
		    {
			sid_t a_sid = actiongraph.graph[tmp]->sid;
			if (s_sid == a_sid && actiongraph.graph[tmp]->last)
			{
			    Partition* p_rhs = dynamic_cast<Partition*>(actiongraph.rhs.get_impl().graph[v_in_rhs].get());
			    Partition* p_lhs = dynamic_cast<Partition*>(actiongraph.lhs.get_impl().graph[q].get());

			    if (p_lhs->get_number() < p_rhs->get_number())
				w.push_back(tmp);
			}
		    }
		}

		if (!w.empty())
		{
		    sort(w.begin(), w.end()); // TODO number sort
		    actiongraph.add_edge(w.back(), v);
		}
	    }
	}


	void
	Delete::add_dependencies(Actiongraph::vertex_descriptor v, Actiongraph& actiongraph) const
	{
	    // all children must be deleted beforehand

	    sid_t sid = actiongraph.graph[v]->sid;

	    Devicegraph::Impl::vertex_descriptor v_in_lhs = actiongraph.lhs.get_impl().find_vertex(sid);

	    // iterate children
	    Devicegraph::Impl::graph_t::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_lhs, actiongraph.lhs.get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t child_sid = actiongraph.rhs.get_impl().graph[*vi]->get_sid();

		for (Actiongraph::vertex_descriptor tmp : actiongraph.huhu(child_sid, true, false))
		    actiongraph.add_edge(v, tmp);
	    }
	}

    }

}
