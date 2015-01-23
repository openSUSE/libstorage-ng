

#include "storage/Action.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/Partition.h"


namespace storage
{

    namespace Action
    {

	Text
	Nop::text(const Actiongraph& actiongraph, bool doing) const
	{
	    return _("Nop");
	}


	Text
	Create::text(const Actiongraph& actiongraph, bool doing) const
	{
	    return device_rhs(actiongraph)->get_impl().do_create_text(doing);
	}


	void
	Create::commit(const Actiongraph& actiongraph) const
	{
	    device_rhs(actiongraph)->get_impl().do_create();
	}


	Text
	Modify::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device_lhs = actiongraph.get_devicegraph(LHS)->find_device(sid);
	    const Device* device_rhs = actiongraph.get_devicegraph(RHS)->find_device(sid);

	    return sformat(_("Rename %1$s to %2$s"), device_lhs->get_displayname().c_str(),
			   device_rhs->get_displayname().c_str());
	}


	Text
	Delete::text(const Actiongraph& actiongraph, bool doing) const
	{
	    return device_lhs(actiongraph)->get_impl().do_delete_text(doing);
	}


	void
	Delete::commit(const Actiongraph& actiongraph) const
	{
	    device_lhs(actiongraph)->get_impl().do_delete();
	}


	void
	Create::add_dependencies(Actiongraph::vertex_descriptor v, Actiongraph& actiongraph) const
	{
	    sid_t sid = actiongraph.graph[v]->sid;

	    Devicegraph::Impl::vertex_descriptor v_in_rhs = actiongraph.get_devicegraph(RHS)->get_impl().find_vertex(sid);

	    // iterate parents
	    Devicegraph::Impl::graph_t::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_rhs, actiongraph.get_devicegraph(RHS)->get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t parent_sid = actiongraph.get_devicegraph(RHS)->get_impl().graph[*vi]->get_sid();

		if (!actiongraph.get_devicegraph(LHS)->device_exists(parent_sid))
		{
		    // parents must be created beforehand if not existed

		    Actiongraph::vertex_descriptor tmp = actiongraph.huhu(parent_sid, false, true).front();
		    actiongraph.add_edge(tmp, v);
		}
		else
		{
		    // children of parents must be deleted beforehand

		    Devicegraph::Impl::vertex_descriptor q = actiongraph.get_devicegraph(LHS)->get_impl().find_vertex(parent_sid);

		    Devicegraph::Impl::graph_t::adjacency_iterator vi2, vi2_end;
		    for (boost::tie(vi2, vi2_end) = adjacent_vertices(q, actiongraph.get_devicegraph(LHS)->get_impl().graph); vi2 != vi2_end; ++vi2)
		    {
			sid_t child_sid = actiongraph.get_devicegraph(LHS)->get_impl().graph[*vi2]->get_sid();

			Actiongraph::vertex_descriptor tmp = actiongraph.huhu(child_sid, false, true).front();
			actiongraph.add_edge(tmp, v);
		    }
		}
	    }

	    // create order of partitions
	    if (dynamic_cast<const Partition*>(actiongraph.get_devicegraph(RHS)->get_impl().graph[v_in_rhs].get()))
	    {
		vector<Devicegraph::Impl::vertex_descriptor> siblings = actiongraph.get_devicegraph(RHS)->get_impl().siblings(v_in_rhs, false);

		vector<Actiongraph::vertex_descriptor> w;

		for (Devicegraph::Impl::vertex_descriptor q : siblings)
		{
		    sid_t s_sid = actiongraph.get_devicegraph(RHS)->get_impl().graph[q]->get_sid();

		    for (Actiongraph::vertex_descriptor tmp : actiongraph.vertices())
		    {
			sid_t a_sid = actiongraph.graph[tmp]->sid;
			if (s_sid == a_sid && actiongraph.graph[tmp]->last)
			{
			    Partition* p_lhs = dynamic_cast<Partition*>(actiongraph.get_devicegraph(LHS)->get_impl().graph[q].get());
			    Partition* p_rhs = dynamic_cast<Partition*>(actiongraph.get_devicegraph(RHS)->get_impl().graph[v_in_rhs].get());

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

	    Devicegraph::Impl::vertex_descriptor v_in_lhs = actiongraph.get_devicegraph(LHS)->get_impl().find_vertex(sid);

	    // iterate children
	    Devicegraph::Impl::graph_t::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_lhs, actiongraph.get_devicegraph(LHS)->get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t child_sid = actiongraph.get_devicegraph(RHS)->get_impl().graph[*vi]->get_sid();

		for (Actiongraph::vertex_descriptor tmp : actiongraph.huhu(child_sid, true, false))
		    actiongraph.add_edge(v, tmp);
	    }
	}

    }

}
