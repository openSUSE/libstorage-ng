

#include "storage/Action.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/Partition.h"


namespace storage
{

    namespace Action
    {

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
		sid_t parent_sid = actiongraph.get_devicegraph(RHS)->get_impl()[*vi]->get_sid();

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
			sid_t child_sid = actiongraph.get_devicegraph(LHS)->get_impl()[*vi2]->get_sid();

			vector<Actiongraph::Impl::vertex_descriptor> tmp = actiongraph.huhu(child_sid, false, true);
			if (!tmp.empty())
			    actiongraph.add_edge(tmp.front(), v);
		    }
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
		sid_t child_sid = actiongraph.get_devicegraph(RHS)->get_impl()[*vi]->get_sid();

		for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.huhu(child_sid, true, false))
		    actiongraph.add_edge(v, tmp);
	    }
	}

    }

}
