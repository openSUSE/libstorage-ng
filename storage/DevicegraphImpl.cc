/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2025] SUSE LLC
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
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>

#include "storage/DevicegraphImpl.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/Disk.h"
#include "storage/Filesystems/Nfs.h"
#include "storage/Filesystems/Tmpfs.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Holders/Holder.h"
#include "storage/StorageImpl.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/GraphvizImpl.h"
#include "storage/Registries.h"


namespace storage
{

    bool
    Devicegraph::Impl::operator==(const Impl& rhs) const
    {
	const set<sid_t> lhs_device_sids = get_device_sids();
	const set<sid_t> rhs_device_sids = rhs.get_device_sids();

	if (lhs_device_sids != rhs_device_sids)
	    return false;

	const set<sid_pair_t> lhs_holder_sids = get_holder_sid_pairs();
	const set<sid_pair_t> rhs_holder_sids = rhs.get_holder_sid_pairs();

	if (lhs_holder_sids != rhs_holder_sids)
	    return false;

	for (sid_t sid : lhs_device_sids)
	{
	    vertex_descriptor lhs_vertex = find_vertex(sid);
	    vertex_descriptor rhs_vertex = rhs.find_vertex(sid);

	    if (*graph[lhs_vertex].get() != *rhs.graph[rhs_vertex].get())
		return false;
	}

	for (sid_pair_t sid_pair : lhs_holder_sids)
	{
	    vector<edge_descriptor> lhs_edges = find_edges(sid_pair);
	    vector<edge_descriptor> rhs_edges = rhs.find_edges(sid_pair);

	    if (!is_permutation(lhs_edges.begin(), lhs_edges.end(), rhs_edges.begin(), rhs_edges.end(),
				[&](edge_descriptor lhs_edge, edge_descriptor rhs_edge) {
				    return *graph[lhs_edge].get() == *rhs.graph[rhs_edge].get();
				}))
		return false;
	}

	return true;
    }


    void
    Devicegraph::Impl::log_diff(std::ostream& log, const Impl& rhs) const
    {
	// TODO

	const set<sid_t> lhs_device_sids = get_device_sids();
	const set<sid_t> rhs_device_sids = rhs.get_device_sids();

	if (lhs_device_sids != rhs_device_sids)
	    log << "device sids differ\n";

	for (sid_t sid : lhs_device_sids)
	{
	    vertex_descriptor lhs_vertex = find_vertex(sid);
	    vertex_descriptor rhs_vertex = rhs.find_vertex(sid);

	    if (*graph[lhs_vertex].get() != *rhs.graph[rhs_vertex].get())
		log << "sid " << sid << " device differ\n";

	    if (graph[lhs_vertex]->get_impl().get_classname() != graph[rhs_vertex]->get_impl().get_classname())
		log << "devices with sid " << sid << " have different types\n";
	    else
		graph[lhs_vertex]->get_impl().log_diff(log, rhs.graph[rhs_vertex]->get_impl());
	}

	const set<sid_pair_t> lhs_holder_sids = get_holder_sid_pairs();
	const set<sid_pair_t> rhs_holder_sids = rhs.get_holder_sid_pairs();

	if (lhs_holder_sids != rhs_holder_sids)
	    log << "holder sid pairs differ\n";

	for (sid_pair_t sid_pair : lhs_holder_sids)
	{
	    vector<edge_descriptor> lhs_edges = find_edges(sid_pair);
	    vector<edge_descriptor> rhs_edges = rhs.find_edges(sid_pair);

	    bool show = false;

	    if (lhs_edges.size() != rhs_edges.size())
	    {
		log << "sid " << sid_pair.first << " " << sid_pair.second << " different number of holders\n";
		show = true;
	    }
	    else if (!is_permutation(lhs_edges.begin(), lhs_edges.end(), rhs_edges.begin(), rhs_edges.end(),
				     [&](edge_descriptor lhs_edge, edge_descriptor rhs_edge) {
					 return *graph[lhs_edge].get() == *rhs.graph[rhs_edge].get();
				     }))
	    {
		log << "sid " << sid_pair.first << " " << sid_pair.second << " holders are not a permutation\n";
		show = true;
	    }

	    if (show)
	    {
		// TODO: It might be worth sorting the holders according to type to give better
		// diffs. But so far having more than one holder is the rare exception.

		size_t n = min(lhs_edges.size(), rhs_edges.size());

		for (size_t i = 0; i < n; ++i)
		{
		    const Holder* lhs_holder = graph[lhs_edges[i]].get();
		    const Holder* rhs_holder = rhs.graph[rhs_edges[i]].get();

		    if (*lhs_holder != *rhs_holder)
			log << "sid " << sid_pair.first << " " << sid_pair.second << " holder pair "
			    << i << " differ\n";

		    if (lhs_holder->get_impl().get_classname() != rhs_holder->get_impl().get_classname())
			log << "sid " << sid_pair.first << " " << sid_pair.second << " holder pair "
			    << i << " have different types\n";
		    else
			lhs_holder->get_impl().log_diff(log, rhs_holder->get_impl());
		}
	    }
	}
    }


    void
    Devicegraph::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	{
	    // check uniqueness of device and holder object and sid

	    // check device and holder back reference

	    set<const Device*> devices;
	    set<const Holder*> holders;
	    set<sid_t> sids;

	    for (vertex_descriptor vertex : vertices())
	    {
		// check uniqueness of device object

		const Device* device = graph[vertex].get();
		if (!devices.insert(device).second)
		    ST_THROW(LogicException("device object not unique within graph"));

		// check uniqueness of device sid

		sid_t sid = device->get_sid();
		if (!sids.insert(sid).second)
		    ST_THROW(LogicException(sformat("sid %d not unique within graph", sid)));

		// check device back reference

		if (&device->get_devicegraph()->get_impl() != this)
		    ST_THROW(LogicException("wrong graph in back references"));

		if (device->get_impl().get_vertex() != vertex)
		    ST_THROW(LogicException("wrong vertex in back references"));
	    }

	    for (edge_descriptor edge : edges())
	    {
		// check uniqueness of holder object

		const Holder* holder = graph[edge].get();
		if (!holders.insert(holder).second)
		    ST_THROW(LogicException("holder object not unique within graph"));

		// check holder back reference

		if (&holder->get_devicegraph()->get_impl() != this)
		    ST_THROW(LogicException("wrong graph in back references"));

		if (holder->get_impl().get_edge() != edge)
		    ST_THROW(LogicException("wrong edge in back references"));
	    }
	}

	{
	    // Look for cycles in the classic view. With btrfs snapshots cycles are possible.

	    filtered_graph_t filtered_graph(graph, make_edge_filter(View::CLASSIC),
					    make_vertex_filter(View::CLASSIC));

	    VertexIndexMapGenerator<filtered_graph_t> vertex_index_map_generator(filtered_graph);

	    bool has_cycle = false;

	    CycleDetector cycle_detector(has_cycle);
	    boost::depth_first_search(filtered_graph, visitor(cycle_detector).
				      vertex_index_map(vertex_index_map_generator.get()));

	    if (has_cycle)
		ST_THROW(Exception("devicegraph has a cycle"));
	}

	{
	    for (vertex_descriptor vertex : vertices())
	    {
		const Device* device = graph[vertex].get();
		device->get_impl().check(check_callbacks);
	    }
	}

	// TODO check that out-edges are consistent, e.g. of same type, only one per Subdevice
	// TODO check that in-edges are consistent, e.g. of same type, exactly one for Partition
	// in general subcheck for each device
    }


    uf_t
    Devicegraph::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	uf_t ret = 0;

	for (vertex_descriptor vertex : vertices())
	{
	    const Device* device = graph[vertex].get();
	    ret |= device->get_impl().used_features(used_features_dependency_type);
	}

	y2mil("used-features: " << get_used_features_dependency_type_name(used_features_dependency_type) <<
	      " " << get_used_features_names(ret));

	return ret;
    }


    bool
    Devicegraph::Impl::is_probed() const
    {
	return &(get_storage()->get_probed()->get_impl()) == this;
    }


    bool
    Devicegraph::Impl::is_staging() const
    {
	return &(get_storage()->get_staging()->get_impl()) == this;
    }


    bool
    Devicegraph::Impl::is_system() const
    {
	return &(get_storage()->get_system()->get_impl()) == this;
    }


    bool
    Devicegraph::Impl::empty() const
    {
	return boost::num_vertices(graph) == 0;
    }


    size_t
    Devicegraph::Impl::num_devices() const
    {
	return boost::num_vertices(graph);
    }


    size_t
    Devicegraph::Impl::num_holders() const
    {
	return boost::num_edges(graph);
    }


    boost::iterator_range<Devicegraph::Impl::vertex_iterator>
    Devicegraph::Impl::vertices() const
    {
	return boost::make_iterator_range(boost::vertices(graph));
    }


    boost::iterator_range<Devicegraph::Impl::edge_iterator>
    Devicegraph::Impl::edges() const
    {
	return boost::make_iterator_range(boost::edges(graph));
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::add_vertex(Device* device)
    {
	return boost::add_vertex(shared_ptr<Device>(device), graph);
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::add_vertex_v2(shared_ptr<Device> device)
    {
	return boost::add_vertex(device, graph);
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::add_edge(vertex_descriptor source_vertex, vertex_descriptor target_vertex,
				Holder* holder)
    {
	// Check that no holder of the same type exists.

	sid_t source_sid = graph[source_vertex]->get_sid();
	sid_t target_sid = graph[target_vertex]->get_sid();

	for (edge_descriptor edge : find_edges(source_sid, target_sid))
	{
	    if (typeid(*graph[edge].get()) == typeid(*holder))
		ST_THROW(HolderAlreadyExists(graph[source_vertex]->get_sid(),
					     graph[target_vertex]->get_sid()));
	}

	pair<Devicegraph::Impl::edge_descriptor, bool> tmp =
	    boost::add_edge(source_vertex, target_vertex, shared_ptr<Holder>(holder), graph);

	// Since parallel edges are allowed tmp.second must always be true.

	if (!tmp.second)
	    ST_THROW(LogicException("boost::add_edge behaved unexpectedly"));

	// TODO should also set devicegraph and edge in holder but the
	// devicegraph is not available here

	return tmp.first;
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::add_edge_v2(vertex_descriptor source_vertex, vertex_descriptor target_vertex,
				   shared_ptr<Holder> holder)
    {
	// Check that no holder of the same type exists.

	sid_t source_sid = graph[source_vertex]->get_sid();
	sid_t target_sid = graph[target_vertex]->get_sid();

	for (edge_descriptor edge : find_edges(source_sid, target_sid))
	{
	    if (typeid(*graph[edge].get()) == typeid(*holder))
		ST_THROW(HolderAlreadyExists(graph[source_vertex]->get_sid(),
					     graph[target_vertex]->get_sid()));
	}

	pair<Devicegraph::Impl::edge_descriptor, bool> tmp =
	    boost::add_edge(source_vertex, target_vertex, holder, graph);

	// Since parallel edges are allowed tmp.second must always be true.

	if (!tmp.second)
	    ST_THROW(LogicException("boost::add_edge behaved unexpectedly"));

	// TODO should also set devicegraph and edge in holder but the
	// devicegraph is not available here

	return tmp.first;
    }


    set<sid_t>
    Devicegraph::Impl::get_device_sids() const
    {
	set<sid_t> sids;

	for (vertex_descriptor vertex : vertices())
	    sids.insert(graph[vertex]->get_sid());

	return sids;
    }


    set<sid_pair_t>
    Devicegraph::Impl::get_holder_sid_pairs() const
    {
	set<sid_pair_t> sids;

	for (edge_descriptor edge : edges())
	    sids.insert(make_pair(graph[edge]->get_source_sid(), graph[edge]->get_target_sid()));

	return sids;
    }


    bool
    Devicegraph::Impl::device_exists(sid_t sid) const
    {
	for (vertex_descriptor vertex : vertices())
	{
	    if (graph[vertex]->get_sid() == sid)
		return true;
	}

	return false;
    }


    bool
    Devicegraph::Impl::holder_exists(sid_t source_sid, sid_t target_sid) const
    {
	for (edge_descriptor edge : edges())
	{
	    if (graph[source(edge)]->get_sid() == source_sid &&
		graph[target(edge)]->get_sid() == target_sid)
		return true;
	}

	return false;
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::find_vertex(sid_t sid) const
    {
	for (vertex_descriptor vertex : vertices())
	{
	    if (graph[vertex]->get_sid() == sid)
		return vertex;
	}

	ST_THROW(DeviceNotFoundBySid(sid));
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::find_edge(sid_t source_sid, sid_t target_sid) const
    {
	vector<Devicegraph::Impl::edge_descriptor> edges = find_edges(source_sid, target_sid);

	if (edges.empty())
	    ST_THROW(HolderNotFoundBySids(source_sid, target_sid));

	if (edges.size() != 1)
	    ST_THROW(WrongNumberOfHolders(edges.size(), 1));

	return edges.front();
    }


    vector<Devicegraph::Impl::edge_descriptor>
    Devicegraph::Impl::find_edges(sid_t source_sid, sid_t target_sid) const
    {
	vector<Devicegraph::Impl::edge_descriptor> ret;

	for (edge_descriptor edge : edges())
	{
	    if (graph[source(edge)]->get_sid() == source_sid &&
		graph[target(edge)]->get_sid() == target_sid)
		ret.push_back(edge);
	}

	return ret;
    }


    vector<Devicegraph::Impl::edge_descriptor>
    Devicegraph::Impl::find_edges(sid_pair_t sid_pair) const
    {
	return find_edges(sid_pair.first, sid_pair.second);
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::set_source(edge_descriptor old_edge, vertex_descriptor source_vertex)
    {
	vertex_descriptor target_vertex = boost::target(old_edge, graph);

	shared_ptr<Holder> new_holder = graph[old_edge].get()->clone_v2();

	Devicegraph::Impl::edge_descriptor new_edge = add_edge_v2(source_vertex, target_vertex,
								  new_holder);

	// set back-reference
	new_holder->get_impl().set_edge(new_edge);

	remove_edge(old_edge);

	Device* target = graph[target_vertex].get();
	target->get_impl().has_new_parent();

	return new_edge;
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::set_target(edge_descriptor old_edge, vertex_descriptor target_vertex)
    {
	vertex_descriptor source_vertex = boost::source(old_edge, graph);

	shared_ptr<Holder> new_holder = graph[old_edge].get()->clone_v2();

	Devicegraph::Impl::edge_descriptor new_edge = add_edge_v2(source_vertex, target_vertex,
								  new_holder);

	// set back-reference
	new_holder->get_impl().set_edge(new_edge);

	remove_edge(old_edge);

	Device* target = graph[target_vertex].get();
	target->get_impl().has_new_parent();

	return new_edge;
    }


    void
    Devicegraph::Impl::clear()
    {
	graph.clear();
    }


    void
    Devicegraph::Impl::remove_vertex(vertex_descriptor vertex)
    {
	boost::clear_vertex(vertex, graph);
	boost::remove_vertex(vertex, graph);
    }


    void
    Devicegraph::Impl::remove_edge(edge_descriptor edge)
    {
	boost::remove_edge(edge, graph);
    }


    size_t
    Devicegraph::Impl::num_children(vertex_descriptor vertex, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	return boost::out_degree(vertex, filtered_graph);
    }


    size_t
    Devicegraph::Impl::num_parents(vertex_descriptor vertex, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	return boost::in_degree(vertex, filtered_graph);
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::child(vertex_descriptor vertex, View view) const
    {
	return target(out_edge(vertex, view));
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::parent(vertex_descriptor vertex, View view) const
    {
	return source(in_edge(vertex, view));
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::children(vertex_descriptor vertex, View view) const
    {
	vector<vertex_descriptor> ret;

	for (edge_descriptor edge : out_edges(vertex, view))
	    ret.push_back(target(edge));

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::parents(vertex_descriptor vertex, View view) const
    {
	vector<vertex_descriptor> ret;

	for (edge_descriptor edge : in_edges(vertex, view))
	    ret.push_back(source(edge));

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::siblings(vertex_descriptor vertex, bool itself, View view) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor parent : parents(vertex, view))
	{
	    for (vertex_descriptor child : children(parent, view))
	    {
		if (itself || vertex != child)
		    ret.push_back(child);
	    }
	}

	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end()), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::descendants(vertex_descriptor vertex, bool itself, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	VertexIndexMapGenerator<filtered_graph_t> vertex_index_map_generator(filtered_graph);

	vector<vertex_descriptor> ret;
	VertexRecorder<vertex_descriptor> vertex_recorder(false, ret);

	boost::breadth_first_search(filtered_graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::ancestors(vertex_descriptor vertex, bool itself, View view) const
    {
	typedef boost::reverse_graph<filtered_graph_t> reverse_graph_t;

	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));
	reverse_graph_t reverse_graph(filtered_graph);

	VertexIndexMapGenerator<reverse_graph_t> vertex_index_map_generator(reverse_graph);

	vector<vertex_descriptor> ret;
	VertexRecorder<vertex_descriptor> vertex_recorder(false, ret);

	boost::breadth_first_search(reverse_graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::leaves(vertex_descriptor vertex, bool itself, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	VertexIndexMapGenerator<filtered_graph_t> vertex_index_map_generator(filtered_graph);

	vector<vertex_descriptor> ret;
	VertexRecorder<vertex_descriptor> vertex_recorder(true, ret);

	boost::breadth_first_search(filtered_graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::roots(vertex_descriptor vertex, bool itself, View view) const
    {
	typedef boost::reverse_graph<filtered_graph_t> reverse_graph_t;

	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));
	reverse_graph_t reverse_graph(filtered_graph);

	VertexIndexMapGenerator<reverse_graph_t> vertex_index_map_generator(filtered_graph);

	vector<vertex_descriptor> ret;
	VertexRecorder<vertex_descriptor> vertex_recorder(true, ret);

	boost::breadth_first_search(reverse_graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::in_edge(vertex_descriptor vertex, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	boost::iterator_range<filtered_graph_t::in_edge_iterator> range =
	    boost::make_iterator_range(boost::in_edges(vertex, filtered_graph));

	size_t size = distance(range.begin(), range.end());
	if (size != 1)
	    ST_THROW(WrongNumberOfParents(size, 1));

	return range.front();
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::out_edge(vertex_descriptor vertex, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	boost::iterator_range<filtered_graph_t::out_edge_iterator> range =
	    boost::make_iterator_range(boost::out_edges(vertex, filtered_graph));

	size_t size = distance(range.begin(), range.end());
	if (size != 1)
	    ST_THROW(WrongNumberOfChildren(size, 1));

	return range.front();
    }


    vector<Devicegraph::Impl::edge_descriptor>
    Devicegraph::Impl::in_edges(vertex_descriptor vertex, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	boost::iterator_range<filtered_graph_t::in_edge_iterator> range =
	    boost::make_iterator_range(boost::in_edges(vertex, filtered_graph));

	return vector<edge_descriptor>(range.begin(), range.end());
    }


    vector<Devicegraph::Impl::edge_descriptor>
    Devicegraph::Impl::out_edges(vertex_descriptor vertex, View view) const
    {
	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	boost::iterator_range<filtered_graph_t::out_edge_iterator> range =
	    boost::make_iterator_range(boost::out_edges(vertex, filtered_graph));

	return vector<edge_descriptor>(range.begin(), range.end());
    }


    void
    Devicegraph::Impl::load(Devicegraph* devicegraph, const string& filename, bool keep_sids)
    {
	if (&devicegraph->get_impl() != this)
	    ST_THROW(LogicException("wrong impl-ptr"));

	clear();

	XmlFile xml(filename);

	const xmlNode* root_node = xml.getRootElement();
	if (!root_node)
	    ST_THROW(Exception("root node not found"));

	const xmlNode* devicegraph_node = getChildNode(root_node, "Devicegraph");
	if (!devicegraph_node)
	    ST_THROW(Exception("Devicegraph node not found"));

	const xmlNode* devices_node = getChildNode(devicegraph_node, "Devices");
	if (devices_node)
	{
	    for (const xmlNode* device_node : getChildNodes(devices_node))
	    {
		const string& classname = (const char*) device_node->parent->name;

		map<string, device_load_fnc>::const_iterator it = device_load_registry.find(classname);
		if (it == device_load_registry.end())
		    ST_THROW(Exception(sformat("unknown device class name %s", classname)));

		const Device* device = it->second(devicegraph, device_node);
		Storage::Impl::raise_global_sid(device->get_sid());
	    }
	}

	const xmlNode* holders_node = getChildNode(devicegraph_node, "Holders");
	if (holders_node)
	{
	    for (const xmlNode* holder_node : getChildNodes(holders_node))
	    {
		const string& classname = (const char*) holder_node->parent->name;

		map<string, holder_load_fnc>::const_iterator it = holder_load_registry.find(classname);
		if (it == holder_load_registry.end())
		    ST_THROW(Exception(sformat("unknown holder class name %s", classname)));

		it->second(devicegraph, holder_node);
	    }
	}

	if (!keep_sids)
	{
	    for (vertex_descriptor vertex : vertices())
	    {
		Device* device = graph[vertex].get();
		device->get_impl().set_sid(Storage::Impl::get_next_sid());
	    }
	}
    }


    void
    Devicegraph::Impl::save(const string& filename) const
    {
	XmlFile xml;

	xmlNode* devicegraph_node = xmlNewNode("Devicegraph");
	xml.setRootElement(devicegraph_node);

	xmlNode* comment = xmlNewComment(string(" " + generated_string() + " ").c_str());
	xmlAddPrevSibling(devicegraph_node, comment);

	xmlNode* devices_node = xmlNewChild(devicegraph_node, "Devices");

	for (vertex_descriptor vertex : vertices())
	{
	    const Device* device = graph[vertex].get();
	    xmlNode* device_node = xmlNewChild(devices_node, device->get_impl().get_classname());
	    device->get_impl().save(device_node);
	}

	xmlNode* holders_node = xmlNewChild(devicegraph_node, "Holders");

	for (edge_descriptor edge : edges())
	{
	    const Holder* holder = graph[edge].get();
	    xmlNode* holder_node = xmlNewChild(holders_node, holder->get_impl().get_classname());
	    holder->get_impl().save(holder_node);
	}

	if (!xml.save_to_file(filename))
	    ST_THROW(Exception(sformat("failed to write '%s'", filename)));
    }


    void
    Devicegraph::Impl::print(std::ostream& out) const
    {
	for (vertex_descriptor vertex : vertices())
	{
	    out << *(graph[vertex].get()) << " -->";

	    vector<sid_t> sids;
	    for (vertex_descriptor child : boost::make_iterator_range(boost::adjacent_vertices(vertex, graph)))
		sids.push_back(graph[child]->get_sid());
	    sort(sids.begin(), sids.end());
	    for (sid_t sid : sids)
		out << " " << sid;

	    out << '\n';
	}

	for (edge_descriptor edge : edges())
	{
	    out << *(graph[edge].get()) << '\n';
	}
    }


    namespace
    {

	struct DevicegraphWriter : public GraphWriter
	{
	    DevicegraphWriter(DevicegraphStyleCallbacks* style_callbacks, const Devicegraph::Impl& devicegraph)
		: style_callbacks(style_callbacks), devicegraph(devicegraph) {}

	    DevicegraphStyleCallbacks* style_callbacks;
	    const Devicegraph::Impl& devicegraph;

	    void operator()(ostream& out) const
	    {
		write_attributes(out, "graph", style_callbacks->graph());
		write_attributes(out, "node", style_callbacks->nodes());
		write_attributes(out, "edge", style_callbacks->edges());

		out << "{ rank=source; ";
		for (const Disk* disk : devicegraph.get_devices_of_type<Disk>())
		    out << disk->get_sid() << "; ";
		for (const Nfs* nfs : devicegraph.get_devices_of_type<Nfs>())
		    out << nfs->get_sid() << "; ";
		for (const Tmpfs* tmpfs : devicegraph.get_devices_of_type<Tmpfs>())
		    out << tmpfs->get_sid() << "; ";
		out << "}\n";

		out << "{ rank=sink; ";
		for (const MountPoint* mount_point : devicegraph.get_devices_of_type<MountPoint>())
		    out << mount_point->get_sid() << "; ";
		out << "}\n";
	    }

	    void operator()(ostream& out, const Devicegraph::Impl::vertex_descriptor& vertex) const
	    {
		const Device* device = devicegraph[vertex];
		write_attributes(out, style_callbacks->node(device));
	    }

	    void operator()(ostream& out, const Devicegraph::Impl::edge_descriptor& edge) const
	    {
		const Holder* holder = devicegraph[edge];
		write_attributes(out, style_callbacks->edge(holder));
	    }
	};

    }


    void
    Devicegraph::Impl::write_graphviz(const string& filename, DevicegraphStyleCallbacks*
				      style_callbacks, View view) const
    {
	ST_CHECK_PTR(style_callbacks);

	filtered_graph_t filtered_graph(graph, make_edge_filter(view), make_vertex_filter(view));

	ofstream fout(filename);

	fout << "// " << generated_string() << "\n\n";

	// Build up a property map with the sid to be used for the
	// vertex id. Same as VertexIndexMapGenerator but with the sid
	// instead of a generated index. Why? For once the sid is
	// needed as id for the ranks. Also other programs can query
	// the id when the user clicks on a node and thus can lookup
	// the device easily.

	typedef map<vertex_descriptor, sid_t> vertex_id_map_t;

	vertex_id_map_t vertex_id_map;

	boost::associative_property_map<vertex_id_map_t> vertex_id_property_map(vertex_id_map);

	for (vertex_descriptor vertex : vertices())
	    boost::put(vertex_id_property_map, vertex, graph[vertex].get()->get_sid());

	const DevicegraphWriter devicegraph_writer(style_callbacks, *this);
	boost::write_graphviz(fout, filtered_graph, devicegraph_writer, devicegraph_writer,
			      devicegraph_writer, vertex_id_property_map);

	fout.close();

	if (!fout.good())
	    ST_THROW(IOException(sformat("failed to write '%s'", filename)));
    }


    Devicegraph::Impl::vertex_filter_t
    Devicegraph::Impl::make_vertex_filter(View view) const
    {
	// graph is needed by reference and view by value
	return [&, view](Devicegraph::Impl::vertex_descriptor vertex) {
	    const Device* device = graph[vertex].get();
	    return device->get_impl().is_in_view(view);
	};
    }


    Devicegraph::Impl::edge_filter_t
    Devicegraph::Impl::make_edge_filter(View view) const
    {
	// graph is needed by reference and view by value
	return [&, view](Devicegraph::Impl::edge_descriptor edge) {
	    const Holder* holder = graph[edge].get();
	    return holder->get_impl().is_in_view(view);
	};
    }

}
