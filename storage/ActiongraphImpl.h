/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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


#ifndef STORAGE_ACTIONGRAPH_IMPL_H
#define STORAGE_ACTIONGRAPH_IMPL_H


#include <deque>
#include <map>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
#include "storage/Actiongraph.h"
#include "storage/Utils/Text.h"
#include "storage/CommitOptions.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::deque;
    using std::map;


    class Devicegraph;
    class Storage;
    class CommitCallbacks;
    class EtcFstab;
    class EtcCrypttab;
    class EtcMdadm;


    namespace Action
    {
	class Base;
    }


    /**
     * Enum to allow filtering of actions in actions_with_sid().
     */
    enum ActionsFilter
    {
	ONLY_FIRST, ONLY_LAST
    };


    class CommitData
    {
    public:

	CommitData(const Actiongraph::Impl& actiongraph, Tense tense);
	~CommitData();

	const Actiongraph::Impl& actiongraph;
	const Tense tense;

	EtcFstab& get_etc_fstab();
	EtcCrypttab& get_etc_crypttab();
	EtcMdadm& get_etc_mdadm();

    private:

	std::unique_ptr<EtcFstab> etc_fstab;
	std::unique_ptr<EtcCrypttab> etc_crypttab;
	std::unique_ptr<EtcMdadm> etc_mdadm;

    };


    class Actiongraph::Impl : private boost::noncopyable
    {

    private:

	typedef boost::adjacency_list<boost::vecS, boost::listS, boost::bidirectionalS,
				      std::shared_ptr<Action::Base>> graph_t;

    public:

	typedef graph_t::vertex_descriptor vertex_descriptor;
	typedef graph_t::edge_descriptor edge_descriptor;

	typedef graph_t::vertex_iterator vertex_iterator;
	typedef graph_t::edge_iterator edge_iterator;

	typedef graph_t::adjacency_iterator adjacency_iterator;
	typedef graph_t::inv_adjacency_iterator inv_adjacency_iterator;

	typedef graph_t::vertices_size_type vertices_size_type;
	typedef graph_t::degree_size_type degree_size_type;

	typedef map<vertex_descriptor, vertices_size_type> vertex_index_map_t;

	Impl(const Storage& storage, Devicegraph* lhs, Devicegraph* rhs);

	const Storage& get_storage() const { return storage; }

	Devicegraph* get_devicegraph(Side side) const { return side == LHS ? lhs : rhs; }

	bool exists_in(const Device* device, Side side) const;

	const Device* find_device(sid_t sid, Side side) const;

	bool empty() const;

	size_t num_actions() const;

	uf_t used_features() const;

	vertex_descriptor add_vertex(const shared_ptr<Action::Base>& action);

	edge_descriptor add_edge(vertex_descriptor a, vertex_descriptor b);

	/**
	 * Adds several actions (with their corresponding vertex_descriptors) to
	 * the graph, linking each other with edges (dependencies) in the given
	 * order. Also sets the first and last flag of the actions.
	 */
	void add_chain(const vector<shared_ptr<Action::Base>>& actions);

	/**
	 * Adds several edges (dependencies) to the graph, linking the actions
	 * in the given order.
	 */
	void add_chain(const vector<vertex_descriptor>& actions);

	/**
	 * Adds several edges (dependencies) to the graph, linking groups of
	 * actions in the given order. Thus, every action from the first vector
	 * will have an edge to every action from the second vector and so on.
	 *
	 * By definition, the cost of this function is O(n^3). Although
	 * creating edges is not an expensive operation, take that cost into
	 * account when creating edges massively.
	 */
	void add_chain(const vector<vector<vertex_descriptor>>& actions);

	const vector<vertex_descriptor>& actions_with_sid(sid_t sid) const;
	vector<vertex_descriptor> actions_with_sid(sid_t sid, ActionsFilter actions_filter) const;

	boost::iterator_range<vertex_iterator> vertices() const;

	boost::iterator_range<adjacency_iterator> children(vertex_descriptor vertex) const;
	boost::iterator_range<inv_adjacency_iterator> parents(vertex_descriptor vertex) const;

	Action::Base* operator[](vertex_descriptor vertex) { return graph[vertex].get(); }
	const Action::Base* operator[](vertex_descriptor vertex) const { return graph[vertex].get(); }

	void print_graph() const;

	void write_graphviz(const string& filename, ActiongraphStyleCallbacks* style_callbacks) const;

	void print_order() const;

	vector<const Action::Base*> get_commit_actions() const;
	void commit(const CommitOptions& commit_options, const CommitCallbacks* commit_callbacks) const;

	void generate_compound_actions(const Actiongraph* actiongraph);
	vector<const CompoundAction*> get_compound_actions() const;

	// special flags, TODO make private and provide interface
	set<sid_t> btrfs_subvolume_delete_is_nop;
	set<sid_t> btrfs_qgroup_delete_is_nop;

	// special actions, TODO make private and provide interface
	vertex_iterator mount_root_filesystem;
	map<sid_t, vertex_descriptor> last_action_on_partition_table;
	map<sid_t, vertex_descriptor> btrfs_enable_quota;

    private:

	void set_gpt_undersized();

	void set_special_flags();
	void get_device_actions();
	void get_holder_actions();
	void remove_duplicates();
	void set_special_actions();
	void add_dependencies();
	void add_mount_dependencies();
	void add_special_dasd_pt_dependencies();
	void remove_only_syncs();
	void set_priorities();
	void set_priorities_upward(vertex_descriptor v1);
	void calculate_order();
	void check_taboos();

	const Storage& storage;

	Devicegraph* lhs;
	Devicegraph* rhs;

	typedef deque<vertex_descriptor> Order;

	Order order;

	class CompareByPriority;

	Order prioritised_topological_sort(const boost::associative_property_map<vertex_index_map_t>& idx) const;

	graph_t graph;

	// map from path to mount/unmount action
	using mount_map_t = map<string, vertex_descriptor>;

	mount_map_t::const_iterator find_mount_parent(const mount_map_t& mount_map,
						      mount_map_t::const_iterator child) const;

	map<sid_t, vector<vertex_descriptor>> cache_for_actions_with_sid;

	vector<shared_ptr<CompoundAction>> compound_actions;

    };

}

#endif
