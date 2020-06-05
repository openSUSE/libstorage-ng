/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#include <boost/graph/copy.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/transitive_reduction.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

#include "storage/Utils/Stopwatch.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/GptImpl.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Action.h"
#include "storage/Actiongraph.h"
#include "storage/StorageImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/EtcFstab.h"
#include "storage/EtcCrypttab.h"
#include "storage/EtcMdadm.h"
#include "storage/CompoundAction/Generator.h"
#include "storage/CommitOptions.h"
#include "storage/Utils/Format.h"
#include "storage/GraphvizImpl.h"


namespace storage
{

    CommitData::CommitData(const Actiongraph::Impl& actiongraph, Tense tense)
	: actiongraph(actiongraph), tense(tense)
    {
    }


    CommitData::~CommitData()
    {
    }


    EtcFstab&
    CommitData::get_etc_fstab()
    {
	if (!etc_fstab)
	{
	    const Storage& storage = actiongraph.get_storage();
	    string filename = storage.get_impl().prepend_rootprefix(ETC_FSTAB);

	    etc_fstab = make_unique<EtcFstab>(filename);
	}

	return *etc_fstab.get();
    }


    EtcCrypttab&
    CommitData::get_etc_crypttab()
    {
	if (!etc_crypttab)
	{
	    const Storage& storage = actiongraph.get_storage();
	    string filename = storage.get_impl().prepend_rootprefix(ETC_CRYPTTAB);

	    etc_crypttab = make_unique<EtcCrypttab>(filename);
	}

	return *etc_crypttab.get();
    }


    EtcMdadm&
    CommitData::get_etc_mdadm()
    {
	if (!etc_mdadm)
	{
	    const Storage& storage = actiongraph.get_storage();
	    string filename = storage.get_impl().prepend_rootprefix(ETC_MDADM);

	    etc_mdadm = make_unique<EtcMdadm>(filename);
	}

	return *etc_mdadm.get();
    }


    class CheckCallbacksLogger : public CheckCallbacks
    {
    public:

	virtual void error(const string& error) const override;

    };


    void
    CheckCallbacksLogger::error(const string& error) const
    {
	y2err(error);
    }


    Actiongraph::Impl::Impl(const Storage& storage, Devicegraph* lhs, Devicegraph* rhs)
	: storage(storage), lhs(lhs), rhs(rhs)
    {
	if (lhs->get_storage() != &storage || rhs->get_storage() != &storage)
	    ST_THROW(Exception("devicegraph belongs to wrong storage object"));

	set_gpt_undersized();

	CheckCallbacksLogger check_callbacks_logger;

	storage.check(&check_callbacks_logger);

	Stopwatch stopwatch;

	get_actions();
	remove_duplicates();
	set_special_actions();
	add_dependencies();
	remove_only_syncs();

	calculate_order();

	y2mil("stopwatch " << stopwatch << " for actiongraph generation");
    }


    void
    Actiongraph::Impl::set_gpt_undersized()
    {
	// Set the undersized attribute to false for GPTs where either a
	// partition is created or grown since those operations might need the
	// GPT to cover the whole device. Without the code here the library
	// user would be responsible to set the attribute.

	for (Gpt* gpt : Gpt::get_all(rhs))
	{
	    if (gpt->get_impl().is_undersized() && gpt->exists_in_devicegraph(lhs))
	    {
		auto might_require_repair = [this](const Partition* partition_rhs)
		{
		    // Check if the partition is created.
		    if (!exists_in(partition_rhs, LHS))
			return true;

		    // Check if the partition is grown.
		    const Partition* partition_lhs = to_partition(find_device(partition_rhs->get_sid(), LHS));
		    if (partition_lhs->get_size() < partition_rhs->get_size())
			return true;

		    return false;
		};

		vector<Partition*> partitions = gpt->get_partitions();
		if (any_of(partitions.begin(), partitions.end(), might_require_repair))
		    gpt->get_impl().set_undersized(false);
	    }
	}
    }


    bool
    Actiongraph::Impl::exists_in(const Device* device, Side side) const
    {
	return device->exists_in_devicegraph(get_devicegraph(side));
    }


    const Device*
    Actiongraph::Impl::find_device(sid_t sid, Side side) const
    {
	return get_devicegraph(side)->find_device(sid);
    }


    bool
    Actiongraph::Impl::empty() const
    {
	return boost::num_vertices(graph) == 0;
    }


    size_t
    Actiongraph::Impl::num_actions() const
    {
	return boost::num_vertices(graph);
    }


    Actiongraph::Impl::vertex_descriptor
    Actiongraph::Impl::add_vertex(Action::Base* action)
    {
	return boost::add_vertex(shared_ptr<Action::Base>(action), graph);
    }


    Actiongraph::Impl::edge_descriptor
    Actiongraph::Impl::add_edge(vertex_descriptor a, vertex_descriptor b)
    {
	pair<edge_descriptor, bool> tmp = boost::add_edge(a, b, graph);

	return tmp.first;
    }


    void
    Actiongraph::Impl::add_chain(const vector<Action::Base*>& actions)
    {
	vertex_descriptor v1 = 0;

	for (vector<Action::Base*>::const_iterator it = actions.begin(); it != actions.end(); ++it)
	{
	    (*it)->first = it == actions.begin();
	    (*it)->last = it == --actions.end();

	    vertex_descriptor v2 = add_vertex(*it);

	    if (it != actions.begin())
		add_edge(v1, v2);

	    v1 = v2;
	}
    }


    void
    Actiongraph::Impl::add_chain(const vector<vertex_descriptor>& actions)
    {
	if (actions.size() > 1)
	{
	    Actiongraph::Impl::vertex_descriptor vertex = actions[0];
	    for (size_t i = 1; i < actions.size(); ++i)
	    {
		add_edge(vertex, actions[i]);
		vertex = actions[i];
	    }
	}
    }


    void
    Actiongraph::Impl::add_chain(const vector<vector<vertex_descriptor>>& vert_vectors)
    {
	vector<vector<Actiongraph::Impl::vertex_descriptor>> non_empty_vectors;

	for (const vector<vertex_descriptor>& vector : vert_vectors)
	    if (!vector.empty())
		non_empty_vectors.push_back(vector);

	if (non_empty_vectors.size() > 1)
	    for (size_t i = 1; i < non_empty_vectors.size(); ++i)
		// As an alternative implementation, we could add sync actions
		// here instead of creating all the edges right away
		for (vertex_descriptor left : non_empty_vectors[i - 1])
		    for (vertex_descriptor right : non_empty_vectors[i])
			add_edge(left, right);
    }


    const vector<Actiongraph::Impl::vertex_descriptor>&
    Actiongraph::Impl::actions_with_sid(sid_t sid) const
    {
	map<sid_t, vector<vertex_descriptor>>::const_iterator it = cache_for_actions_with_sid.find(sid);
	if (it != cache_for_actions_with_sid.end())
	    return it->second;

	const static vector<vertex_descriptor> empty;
	return empty;
    }


    vector<Actiongraph::Impl::vertex_descriptor>
    Actiongraph::Impl::actions_with_sid(sid_t sid, ActionsFilter actions_filter) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor vertex : actions_with_sid(sid))
	{
	    const Action::Base* action = graph[vertex].get();

	    if (actions_filter == ONLY_FIRST && !action->first)
		continue;

	    if (actions_filter == ONLY_LAST && !action->last)
		continue;

	    ret.push_back(vertex);
	}

	return ret;
    }


    boost::iterator_range<Actiongraph::Impl::vertex_iterator>
    Actiongraph::Impl::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    boost::iterator_range<Actiongraph::Impl::adjacency_iterator>
    Actiongraph::Impl::children(vertex_descriptor vertex) const
    {
	return boost::make_iterator_range(boost::adjacent_vertices(vertex, graph));
    }


    boost::iterator_range<Actiongraph::Impl::inv_adjacency_iterator>
    Actiongraph::Impl::parents(vertex_descriptor vertex) const
    {
	return boost::make_iterator_range(boost::inv_adjacent_vertices(vertex, graph));
    }


    void
    Actiongraph::Impl::get_actions()
    {
	const set<sid_t> lhs_sids = lhs->get_impl().get_device_sids();
	const set<sid_t> rhs_sids = rhs->get_impl().get_device_sids();

	vector<sid_t> created_sids;
	set_difference(rhs_sids.begin(), rhs_sids.end(), lhs_sids.begin(), lhs_sids.end(),
		       back_inserter(created_sids));

	vector<sid_t> common_sids;
	set_intersection(lhs_sids.begin(), lhs_sids.end(), rhs_sids.begin(), rhs_sids.end(),
			 back_inserter(common_sids));

	vector<sid_t> deleted_sids;
	set_difference(lhs_sids.begin(), lhs_sids.end(), rhs_sids.begin(), rhs_sids.end(),
		       back_inserter(deleted_sids));

	for (sid_t sid : created_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_rhs = rhs->get_impl().find_vertex(sid);
	    const Device* d_rhs = rhs->get_impl()[v_rhs];

	    d_rhs->get_impl().add_create_actions(*this);
	}

	for (sid_t sid : common_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    const Device* d_lhs = lhs->get_impl()[v_lhs];

	    Devicegraph::Impl::vertex_descriptor v_rhs = rhs->get_impl().find_vertex(sid);
	    const Device* d_rhs = rhs->get_impl()[v_rhs];

	    d_rhs->get_impl().add_modify_actions(*this, d_lhs);
	}

	for (sid_t sid : deleted_sids)
	{
	    Devicegraph::Impl::vertex_descriptor v_lhs = lhs->get_impl().find_vertex(sid);
	    const Device* d_lhs = lhs->get_impl()[v_lhs];

	    d_lhs->get_impl().add_delete_actions(*this);
	}
    }


    /**
     * It is required in many cases to deactivate a mount point. This function
     * removes duplicate mount and unmount actions while copying the
     * dependencies. Thus the code inserting mount and unmount actions does
     * not have to check for duplicates.
     */
    void
    Actiongraph::Impl::remove_duplicates()
    {
	vector<pair<vertex_descriptor, vertex_descriptor>> duplicates;

	boost::iterator_range<vertex_iterator> range = vertices();

	for (vertex_iterator it1 = range.begin(); it1 != range.end(); ++it1)
	{
	    const Action::Base* action1 = graph[*it1].get();

	    for (vertex_iterator it2 = next(it1); it2 != range.end(); ++it2)
	    {
		const Action::Base* action2 = graph[*it2].get();

		if (action1->sid == action2->sid)
		{
		    if ((is_mount(action1) && is_mount(action2)) ||
			(is_unmount(action1) && is_unmount(action2)))
			duplicates.push_back(make_pair(*it1, *it2));
		}
	    }
	}

	for (pair<vertex_descriptor, vertex_descriptor> duplicate : duplicates)
	{
	    for (vertex_descriptor parent : parents(duplicate.second))
		add_edge(parent, duplicate.first);

	    for (vertex_descriptor child : children(duplicate.second))
		add_edge(duplicate.first, child);

	    clear_vertex(duplicate.second, graph);
	    remove_vertex(duplicate.second, graph);
	}
    }


    void
    Actiongraph::Impl::set_special_actions()
    {
	boost::iterator_range<vertex_iterator> range = vertices();

	mount_root_filesystem = range.end();

	for (vertex_iterator it = range.begin(); it != range.end(); ++it)
	{
	    const Action::Base* action = graph[*it].get();

	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(action);
	    if (mount && mount->get_path(*this) == "/")
		mount_root_filesystem = it;

	    cache_for_actions_with_sid[action->sid].push_back(*it);
	}
    }


    void
    Actiongraph::Impl::add_dependencies()
    {
	vector<vertex_descriptor> mounts;

	// TODO also for Devices only in LHS?

	const Devicegraph* devicegraph = get_devicegraph(RHS);

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const Device* device = devicegraph->get_impl()[vertex];

	    device->get_impl().add_dependencies(*this);
	}

	PartitionTable::Impl::run_dependency_manager(*this);

	Bcache::Impl::run_dependency_manager(*this);

	for (vertex_descriptor vertex : vertices())
	{
	    graph[vertex]->add_dependencies(vertex, *this);

	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(graph[vertex].get());
	    if (mount && mount->get_path(*this) != "swap")
		mounts.push_back(vertex);
	}

	if (mounts.size() > 1)
	{
	    // TODO correct sort
	    sort(mounts.begin(), mounts.end(), [this, &mounts](vertex_descriptor l, vertex_descriptor r) {
		const Action::Mount* ml = dynamic_cast<const Action::Mount*>(graph[l].get());
		const Action::Mount* mr = dynamic_cast<const Action::Mount*>(graph[r].get());
		return ml->get_path(*this) <= mr->get_path(*this);
	    });

	    add_chain(mounts);
	}

	add_special_dasd_pt_dependencies();
    }


    void
    Actiongraph::Impl::add_special_dasd_pt_dependencies()
    {
	// For DASD partition tables parted always uses the BLKRRPART ioctl
	// instead of BLKPG_*_PARTITION ioctl. So no partition must be in use
	// when modifying the creating/resizing/deleting partitions.

	// For all mount actions add a dependency to the last action on the
	// partition table.

	// TODO Equivalent for all actions that use/unuse a partition. Or find
	// a better solution.

	for (vertex_descriptor vertex : vertices())
	{
	    const Action::Mount* mount = dynamic_cast<const Action::Mount*>(graph[vertex].get());
	    if (!mount)
		continue;

	    const MountPoint* mount_point = mount->get_mount_point(*this);
	    if (!mount_point->has_mountable())
		continue;

	    const Mountable* mountable = mount_point->get_mountable();
	    if (!mountable->has_filesystem())
		continue;

	    const Filesystem* filesystem = mountable->get_filesystem();
	    if (!is_blk_filesystem(filesystem))
		continue;

	    const BlkFilesystem* blk_filesystem = to_blk_filesystem(filesystem);
	    for (const BlkDevice* blk_device : blk_filesystem->get_blk_devices())
	    {
		if (!is_partition(blk_device))
		    continue;

		const Partition* partition = to_partition(blk_device);

		const PartitionTable* partition_table = partition->get_partition_table();
		if (partition_table->get_type() != PtType::DASD)
		    continue;

		map<sid_t, vertex_descriptor>::const_iterator it1 =
		    last_action_on_partition_table.find(partition_table->get_sid());
		if (it1 == last_action_on_partition_table.end())
		    continue;

		add_edge(it1->second, vertex);
	    }
	}
    }


    void
    Actiongraph::Impl::remove_only_syncs()
    {
	vector<vertex_descriptor> only_syncs;

	for (vertex_descriptor vertex : vertices())
	{
	    if ((*this)[vertex]->only_sync)
		only_syncs.push_back(vertex);
	}

	for (vertex_descriptor vertex : only_syncs)
	{
	    for (vertex_descriptor parent : parents(vertex))
		for (vertex_descriptor child : children(vertex))
		    add_edge(parent, child);

	    clear_vertex(vertex, graph);
	    remove_vertex(vertex, graph);
	}
    }


    void
    Actiongraph::Impl::calculate_order()
    {
	VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	try
	{
	    boost::topological_sort(graph, front_inserter(order),
				    vertex_index_map(vertex_index_map_generator.get()));
	}
	catch (const boost::not_a_dag&)
	{
	    ST_THROW(Exception("actiongraph not a DAG"));
	}
    }


    void
    Actiongraph::Impl::print_order() const
    {
	const CommitData commit_data(*this, Tense::SIMPLE_PRESENT);

	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();
	    cout << action->text(commit_data).native << '\n';
	}

	cout << '\n';
    }


    vector<const Action::Base*>
    Actiongraph::Impl::get_commit_actions() const
    {
	vector<const Action::Base*> commit_actions;

	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();

	    commit_actions.push_back(action);
	}

	return commit_actions;
    }


    void
    Actiongraph::Impl::commit(const CommitOptions& commit_options, const CommitCallbacks* commit_callbacks) const
    {
	y2mil("commit begin");

	CommitData commit_data(*this, Tense::PRESENT_CONTINUOUS);

	for (const vertex_descriptor& vertex : order)
	{
	    const Action::Base* action = graph[vertex].get();

	    Text text = action->text(commit_data);

	    y2mil("Commit Action \"" << text.native << "\" [" << action->details() << "]");

	    message_callback(commit_callbacks, text);

	    if (action->nop)
		continue;

	    try
	    {
		action->commit(commit_data, commit_options);
	    }
	    catch (const Exception& exception)
	    {
		ST_CAUGHT(exception);

		error_callback(commit_callbacks, text, exception);
	    }
	}

	y2mil("commit end");
    }


    void
    Actiongraph::Impl::generate_compound_actions(const Actiongraph* actiongraph)
    {
	compound_actions.clear();

	auto compound_actions = CompoundAction::Generator(actiongraph).generate();
	for (auto action : compound_actions)
	    Impl::compound_actions.push_back(shared_ptr<CompoundAction>(action));
    }


    vector<const CompoundAction*>
    Actiongraph::Impl::get_compound_actions() const
    {
	vector<const CompoundAction*> ret;
	for (auto compound_action : compound_actions)
	    ret.push_back(compound_action.get());

	return ret;
    }


    void
    Actiongraph::Impl::print_graph() const
    {
	typedef map<vertex_descriptor, string> vertex_name_map_t;
	vertex_name_map_t vertex_name_map;
	boost::associative_property_map<vertex_name_map_t> my_vertex_name_map(vertex_name_map);

	const CommitData commit_data(*this, Tense::SIMPLE_PRESENT);

	for (vertex_descriptor v : vertices())
	{
	    string text = "[ " + graph[v]->text(commit_data).translated + " ]";
	    boost::put(my_vertex_name_map, v, text);
	}

	boost::print_graph(graph, my_vertex_name_map);

	cout << '\n';
    }


    namespace
    {

	struct ActiongraphWriter : private GraphWriter
	{
	    ActiongraphWriter(ActiongraphStyleCallbacks* style_callbacks, const CommitData& commit_data)
		: style_callbacks(style_callbacks), commit_data(commit_data) {}

	    ActiongraphStyleCallbacks* style_callbacks;
	    const CommitData& commit_data;

	    void operator()(ostream& out) const
	    {
		write_attributes(out, "graph", style_callbacks->graph());
		write_attributes(out, "node", style_callbacks->nodes());
		write_attributes(out, "edge", style_callbacks->edges());
	    }

	    void operator()(ostream& out, const Actiongraph::Impl::vertex_descriptor& vertex) const
	    {
		const Action::Base* action = commit_data.actiongraph[vertex];
		write_attributes(out, style_callbacks->node(commit_data, action));
	    }

	    void operator()(ostream& out, const Actiongraph::Impl::edge_descriptor& edge) const
	    {
	    }
	};

    }


    void
    Actiongraph::Impl::write_graphviz(const string& filename, ActiongraphStyleCallbacks* style_callbacks) const
    {
	ST_CHECK_PTR(style_callbacks);

	ofstream fout(filename);

	fout << "// " << generated_string() << "\n\n";

	VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	const CommitData commit_data(*this, Tense::SIMPLE_PRESENT);

	const ActiongraphWriter actiongraph_writer(style_callbacks, commit_data);
	boost::write_graphviz(fout, graph, actiongraph_writer, actiongraph_writer, actiongraph_writer,
			      vertex_index_map_generator.get());

	fout.close();

	if (!fout.good())
	    ST_THROW(Exception(sformat("failed to write '%s'", filename)));
    }

}
