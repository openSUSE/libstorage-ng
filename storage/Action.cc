

#include "storage/Action.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/Partition.h"


namespace storage
{

    namespace Action
    {

	Text
	Nop::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    return sformat(_("Nop [sid:%1$d]"), sid);
	}


	Text
	Create::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    return device_rhs(actiongraph)->get_impl().do_create_text(doing);
	}


	void
	Create::commit(const Actiongraph::Impl& actiongraph) const
	{
	    device_rhs(actiongraph)->get_impl().do_create();
	}


	Text
	Delete::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    return device_lhs(actiongraph)->get_impl().do_delete_text(doing);
	}


	void
	Delete::commit(const Actiongraph::Impl& actiongraph) const
	{
	    device_lhs(actiongraph)->get_impl().do_delete();
	}


	void
	Create::add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const
	{
	    sid_t sid = actiongraph[v]->sid;

	    Devicegraph::Impl::vertex_descriptor v_in_rhs = actiongraph.get_devicegraph(RHS)->get_impl().find_vertex(sid);

	    // iterate parents
	    Devicegraph::Impl::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_rhs, actiongraph.get_devicegraph(RHS)->get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t parent_sid = actiongraph.get_devicegraph(RHS)->get_impl().graph[*vi]->get_sid();

		if (!actiongraph.get_devicegraph(LHS)->device_exists(parent_sid))
		{
		    // parents must be created beforehand if not existed

		    Actiongraph::Impl::vertex_descriptor tmp = actiongraph.huhu(parent_sid, false, true).front();
		    actiongraph.add_edge(tmp, v);
		}
		else
		{
		    // children of parents must be deleted beforehand

		    Devicegraph::Impl::vertex_descriptor q = actiongraph.get_devicegraph(LHS)->get_impl().find_vertex(parent_sid);

		    Devicegraph::Impl::adjacency_iterator vi2, vi2_end;
		    for (boost::tie(vi2, vi2_end) = adjacent_vertices(q, actiongraph.get_devicegraph(LHS)->get_impl().graph); vi2 != vi2_end; ++vi2)
		    {
			sid_t child_sid = actiongraph.get_devicegraph(LHS)->get_impl().graph[*vi2]->get_sid();

			vector<Actiongraph::Impl::vertex_descriptor> tmp = actiongraph.huhu(child_sid, false, true);
			if (!tmp.empty())
			    actiongraph.add_edge(tmp.front(), v);
		    }
		}
	    }

	    // create order of partitions
	    if (dynamic_cast<const Partition*>(actiongraph.get_devicegraph(RHS)->get_impl().graph[v_in_rhs].get()))
	    {
		vector<Devicegraph::Impl::vertex_descriptor> siblings = actiongraph.get_devicegraph(RHS)->get_impl().siblings(v_in_rhs, false);

		vector<Actiongraph::Impl::vertex_descriptor> w;

		for (Devicegraph::Impl::vertex_descriptor q : siblings)
		{
		    sid_t s_sid = actiongraph.get_devicegraph(RHS)->get_impl().graph[q]->get_sid();

		    for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.vertices())
		    {
			sid_t a_sid = actiongraph[tmp]->sid;
			if (s_sid == a_sid && actiongraph[tmp]->last)
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
	Delete::add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const
	{
	    // all children must be deleted beforehand

	    sid_t sid = actiongraph[v]->sid;

	    Devicegraph::Impl::vertex_descriptor v_in_lhs = actiongraph.get_devicegraph(LHS)->get_impl().find_vertex(sid);

	    // iterate children
	    Devicegraph::Impl::inv_adjacency_iterator vi, vi_end;
	    for (boost::tie(vi, vi_end) = inv_adjacent_vertices(v_in_lhs, actiongraph.get_devicegraph(LHS)->get_impl().graph); vi != vi_end; ++vi)
	    {
		sid_t child_sid = actiongraph.get_devicegraph(RHS)->get_impl().graph[*vi]->get_sid();

		for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.huhu(child_sid, true, false))
		    actiongraph.add_edge(v, tmp);
	    }
	}

    }

}
