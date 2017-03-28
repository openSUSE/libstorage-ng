/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Luks.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Filesystems/Ext2.h"
#include "storage/Filesystems/Ext3.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Ntfs.h"
#include "storage/Filesystems/Vfat.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsSubvolume.h"
#include "storage/Filesystems/Reiserfs.h"
#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Filesystems/Iso9660.h"
#include "storage/Filesystems/Udf.h"
#include "storage/Filesystems/Nfs.h"
#include "storage/Holders/HolderImpl.h"
#include "storage/Holders/User.h"
#include "storage/Holders/MdUser.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Storage.h"
#include "storage/FreeInfo.h"


namespace storage
{

    bool
    Devicegraph::Impl::operator==(const Impl& rhs) const
    {
	const set<sid_t> lhs_device_sids = get_device_sids();
	const set<sid_t> rhs_device_sids = rhs.get_device_sids();

	if (lhs_device_sids != rhs_device_sids)
	    return false;

	const set<pair<sid_t, sid_t>> lhs_holder_sids = get_holder_sids();
	const set<pair<sid_t, sid_t>> rhs_holder_sids = rhs.get_holder_sids();

	if (lhs_holder_sids != rhs_holder_sids)
	    return false;

	for (sid_t sid : lhs_device_sids)
	{
	    vertex_descriptor lhs_vertex = find_vertex(sid);
	    vertex_descriptor rhs_vertex = rhs.find_vertex(sid);

	    if (*graph[lhs_vertex].get() != *rhs.graph[rhs_vertex].get())
		return false;
	}

	for (pair<sid_t, sid_t> sid : lhs_holder_sids)
	{
	    edge_descriptor lhs_edge = find_edge(sid.first, sid.second);
	    edge_descriptor rhs_edge = rhs.find_edge(sid.first, sid.second);

	    if (*graph[lhs_edge].get() != *rhs.graph[rhs_edge].get())
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
	    log << "device sids differ" << endl;

	for (sid_t sid : lhs_device_sids)
	{
	    vertex_descriptor lhs_vertex = find_vertex(sid);
	    vertex_descriptor rhs_vertex = rhs.find_vertex(sid);

	    if (*graph[lhs_vertex].get() != *rhs.graph[rhs_vertex].get())
		log << "sid " << sid << " device differ" << endl;

	    graph[lhs_vertex]->get_impl().log_diff(log, rhs.graph[rhs_vertex]->get_impl());
	}

	const set<pair<sid_t, sid_t>> lhs_holder_sids = get_holder_sids();
	const set<pair<sid_t, sid_t>> rhs_holder_sids = rhs.get_holder_sids();

	if (lhs_holder_sids != rhs_holder_sids)
	    log << "holder sids differ" << endl;

	for (pair<sid_t, sid_t> sid : lhs_holder_sids)
	{
	    edge_descriptor lhs_edge = find_edge(sid.first, sid.second);
	    edge_descriptor rhs_edge = rhs.find_edge(sid.first, sid.second);

	    if (*graph[lhs_edge].get() != *rhs.graph[rhs_edge].get())
		log << "sid " << sid.first << " " << sid.second << " holder differ" << endl;

	    graph[lhs_edge]->get_impl().log_diff(log, rhs.graph[rhs_edge]->get_impl());
	}
    }


    void
    Devicegraph::Impl::check() const
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
		    ST_THROW(LogicException("sid not unique within graph"));

		// check device back reference

		if (&device->get_impl().get_devicegraph()->get_impl() != this)
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

		if (&holder->get_impl().get_devicegraph()->get_impl() != this)
		    ST_THROW(LogicException("wrong graph in back references"));

		if (holder->get_impl().get_edge() != edge)
		    ST_THROW(LogicException("wrong edge in back references"));
	    }
	}

	{
	    // look for cycles

	    VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	    bool has_cycle = false;

	    CycleDetector cycle_detector(has_cycle);
	    boost::depth_first_search(graph, visitor(cycle_detector).
				      vertex_index_map(vertex_index_map_generator.get()));

	    if (has_cycle)
		ST_THROW(Exception("devicegraph has a cycle"));
	}

	{
	    for (vertex_descriptor vertex : vertices())
	    {
		const Device* device = graph[vertex].get();
		device->get_impl().check();
	    }
	}

	// TODO check that out-edges are consistent, e.g. of same type, only one per Subdevice
	// TODO check that in-edges are consistent, e.g. of same type, exactly one for Partition
	// in general subcheck for each device
    }


    uint64_t
    Devicegraph::Impl::used_features() const
    {
	uint64_t ret = 0;

	for (vertex_descriptor vertex : vertices())
	{
	    const Device* device = graph[vertex].get();
	    ret |= device->get_impl().used_features();
	}

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


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::add_edge(vertex_descriptor source_vertex, vertex_descriptor target_vertex,
				Holder* holder)
    {
	pair<Devicegraph::Impl::edge_descriptor, bool> tmp =
	    boost::add_edge(source_vertex, target_vertex, shared_ptr<Holder>(holder), graph);

	if (!tmp.second)
	    ST_THROW(HolderAlreadyExists(graph[source_vertex]->get_sid(),
					 graph[target_vertex]->get_sid()));

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


    set<pair<sid_t, sid_t>>
    Devicegraph::Impl::get_holder_sids() const
    {
	set<pair<sid_t, sid_t>> sids;
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
	for (edge_descriptor edge : edges())
	{
	    if (graph[source(edge)]->get_sid() == source_sid &&
		graph[target(edge)]->get_sid() == target_sid)
		return edge;
	}

	ST_THROW(HolderNotFoundBySid(source_sid, target_sid));
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


    void
    Devicegraph::Impl::swap(Devicegraph::Impl& x)
    {
	graph.swap(x.graph);
    }


    size_t
    Devicegraph::Impl::num_children(vertex_descriptor vertex) const
    {
	return boost::out_degree(vertex, graph);
    }


    size_t
    Devicegraph::Impl::num_parents(vertex_descriptor vertex) const
    {
	return boost::in_degree(vertex, graph);
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::child(vertex_descriptor vertex) const
    {
	if (num_children(vertex) != 1)
	    ST_THROW(WrongNumberOfChildren(num_children(vertex), 1));

	return *boost::make_iterator_range(boost::adjacent_vertices(vertex, graph)).begin();
    }


    Devicegraph::Impl::vertex_descriptor
    Devicegraph::Impl::parent(vertex_descriptor vertex) const
    {
	if (num_parents(vertex) != 1)
	    ST_THROW(WrongNumberOfParents(num_children(vertex), 1));

	return *boost::make_iterator_range(boost::inv_adjacent_vertices(vertex, graph)).begin();
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::children(vertex_descriptor vertex) const
    {
	boost::iterator_range<adjacency_iterator> range =
	    boost::make_iterator_range(boost::adjacent_vertices(vertex, graph));
	return vector<vertex_descriptor>(range.begin(), range.end());
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::parents(vertex_descriptor vertex) const
    {
	boost::iterator_range<inv_adjacency_iterator> range =
	    boost::make_iterator_range(boost::inv_adjacent_vertices(vertex, graph));
	return vector<vertex_descriptor>(range.begin(), range.end());
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::siblings(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	for (vertex_descriptor tmp1 : boost::make_iterator_range(inv_adjacent_vertices(vertex, graph)))
	{
	    for (vertex_descriptor tmp2 : boost::make_iterator_range(adjacent_vertices(tmp1, graph)))
	    {
		if (itself || vertex != tmp2)
		    ret.push_back(tmp2);
	    }
	}

	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end()), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::descendants(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	VertexRecorder<vertex_descriptor> vertex_recorder(false, ret);

	boost::breadth_first_search(graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::ancestors(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	typedef boost::reverse_graph<graph_t> reverse_graph_t;

	reverse_graph_t reverse_graph(graph);

	VertexIndexMapGenerator<reverse_graph_t> vertex_index_map_generator(reverse_graph);

	VertexRecorder<vertex_descriptor> vertex_recorder(false, ret);

	boost::breadth_first_search(reverse_graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::leaves(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	VertexRecorder<vertex_descriptor> vertex_recorder(true, ret);

	boost::breadth_first_search(graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    vector<Devicegraph::Impl::vertex_descriptor>
    Devicegraph::Impl::roots(vertex_descriptor vertex, bool itself) const
    {
	vector<vertex_descriptor> ret;

	typedef boost::reverse_graph<graph_t> reverse_graph_t;

	reverse_graph_t reverse_graph(graph);

	VertexIndexMapGenerator<reverse_graph_t> vertex_index_map_generator(graph);

	VertexRecorder<vertex_descriptor> vertex_recorder(true, ret);

	boost::breadth_first_search(reverse_graph, vertex, visitor(vertex_recorder).
				    vertex_index_map(vertex_index_map_generator.get()));

	if (!itself)
	    ret.erase(remove(ret.begin(), ret.end(), vertex), ret.end());

	return ret;
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::in_edge(vertex_descriptor vertex) const
    {
	if (num_children(vertex) != 1)
	    ST_THROW(WrongNumberOfChildren(num_children(vertex), 1));

	return *boost::make_iterator_range(boost::in_edges(vertex, graph)).begin();
    }


    Devicegraph::Impl::edge_descriptor
    Devicegraph::Impl::out_edge(vertex_descriptor vertex) const
    {
	if (num_parents(vertex) != 1)
	    ST_THROW(WrongNumberOfParents(num_parents(vertex), 1));

	return *boost::make_iterator_range(boost::out_edges(vertex, graph)).begin();
    }


    vector<Devicegraph::Impl::edge_descriptor>
    Devicegraph::Impl::in_edges(vertex_descriptor vertex) const
    {
	boost::iterator_range<in_edge_iterator> range =
	    boost::make_iterator_range(boost::in_edges(vertex, graph));
	return vector<edge_descriptor>(range.begin(), range.end());
    }


    vector<Devicegraph::Impl::edge_descriptor>
    Devicegraph::Impl::out_edges(vertex_descriptor vertex) const
    {
	boost::iterator_range<out_edge_iterator> range =
	    boost::make_iterator_range(boost::out_edges(vertex, graph));
	return vector<edge_descriptor>(range.begin(), range.end());
    }


    typedef std::function<Device* (Devicegraph* devicegraph, const xmlNode* node)> device_load_fnc;

    const map<string, device_load_fnc> device_load_registry = {
	{ "Disk", &Disk::load },
	{ "Dasd", &Dasd::load },
	{ "Md", &Md::load },
	{ "Msdos", &Msdos::load },
	{ "Gpt", &Gpt::load },
	{ "Partition", &Partition::load },
	{ "LvmPv", &LvmPv::load },
	{ "LvmVg", &LvmVg::load },
	{ "LvmLv", &LvmLv::load },
	{ "Encryption", &Encryption::load },
	{ "Luks", &Luks::load },
	{ "Bcache", &Bcache::load },
	{ "BcacheCset", &BcacheCset::load },
	{ "Ext2", &Ext2::load },
	{ "Ext3", &Ext3::load },
	{ "Ext4", &Ext4::load },
	{ "Ntfs", &Ntfs::load },
	{ "Vfat", &Vfat::load },
	{ "Btrfs", &Btrfs::load },
	{ "BtrfsSubvolume", &BtrfsSubvolume::load },
	{ "Reiserfs", &Reiserfs::load },
	{ "Xfs", &Xfs::load },
	{ "Swap", &Swap::load },
	{ "Iso9660", &Iso9660::load },
	{ "Udf", &Udf::load },
	{ "Nfs", &Nfs::load }
    };


    typedef std::function<Holder* (Devicegraph* devicegraph, const xmlNode* node)> holder_load_fnc;

    const map<string, holder_load_fnc> holder_load_registry = {
	{ "User", &User::load },
	{ "MdUser", &MdUser::load },
	{ "Subdevice", &Subdevice::load }
    };


    void
    Devicegraph::Impl::load(Devicegraph* devicegraph, const string& filename)
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
		    ST_THROW(Exception("unknown device class name"));

		it->second(devicegraph, device_node);
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
		    ST_THROW(Exception("unknown holder class name"));

		it->second(devicegraph, holder_node);
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
	    device->save(device_node);
	}

	xmlNode* holders_node = xmlNewChild(devicegraph_node, "Holders");

	for (edge_descriptor edge : edges())
	{
	    const Holder* holder = graph[edge].get();
	    xmlNode* holder_node = xmlNewChild(holders_node, holder->get_impl().get_classname());
	    holder->save(holder_node);
	}

	xml.save(filename);
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

	    out << endl;
	}

	for (edge_descriptor edge : edges())
	{
	    out << *(graph[edge].get()) << endl;
	}
    }


    namespace
    {

	struct write_graph
	{
	    write_graph(const Devicegraph::Impl&) {}

	    void operator()(ostream& out) const
	    {
		out << "node [ shape=rectangle, style=filled, fontname=\"Arial\" ];" << '\n';
		out << "edge [ color=\"#444444\" ];" << '\n';
	    }
	};


	struct write_vertex
	{
	    write_vertex(const Devicegraph::Impl& devicegraph, GraphvizFlags graphviz_flags)
		: devicegraph(devicegraph), graphviz_flags(graphviz_flags) {}

	    const Devicegraph::Impl& devicegraph;
	    const GraphvizFlags graphviz_flags;

	    void operator()(ostream& out, const Devicegraph::Impl::vertex_descriptor& vertex) const
	    {
		const Device* device = devicegraph[vertex];

		string label = device->get_displayname();
		string tooltip = device->get_displayname();

		string& extra = (graphviz_flags && GraphvizFlags::TOOLTIP) ? tooltip : label;

		if (graphviz_flags && GraphvizFlags::CLASSNAME)
		{
		    extra += "\\n" + string(device->get_impl().get_classname());
		}

		if (graphviz_flags && GraphvizFlags::SID)
		{
		    extra += "\\n" "sid:" + to_string(device->get_sid());
		}

		if (graphviz_flags && GraphvizFlags::SIZE)
		{
		    if (is_blk_device(device))
		    {
			const BlkDevice* blk_device = to_blk_device(device);
			extra += "\\n" + blk_device->get_size_string();
		    }
		    else if (is_lvm_vg(device))
		    {
			const LvmVg* lvm_vg = to_lvm_vg(device);
			extra += "\\n" + lvm_vg->get_size_string();
		    }
		    else if (is_filesystem(device))
		    {
			const Filesystem* filesystem = to_filesystem(device);
			if (filesystem->has_space_info())
			    extra += "\\n" + filesystem->detect_space_info().get_size_string();
		    }
		}

		out << "[ label=" << boost::escape_dot_string(label);

		if (is_disk(device) || is_dasd(device))
		    out << ", color=\"#ff0000\", fillcolor=\"#ffaaaa\"";
		else if (is_md(device))
		    out << ", color=\"#aaaa00\", fillcolor=\"#ffffaa\"";
		else if (is_partition_table(device))
		    out << ", color=\"#ff0000\", fillcolor=\"#ffaaaa\"";
		else if (is_partition(device))
		    out << ", color=\"#cc33cc\", fillcolor=\"#eeaaee\"";
		else if (is_lvm_pv(device))
		    out << ", color=\"#66dd22\", fillcolor=\"#bbff99\"";
		else if (is_lvm_vg(device))
		    out << ", color=\"#0000ff\", fillcolor=\"#aaaaff\"";
		else if (is_lvm_lv(device))
		    out << ", color=\"#6622dd\", fillcolor=\"#bb99ff\"";
		else if (is_encryption(device))
		    out << ", color=\"#6622dd\", fillcolor=\"#bb99ff\"";
		else if (is_bcache(device))
		    out << ", color=\"#6622dd\", fillcolor=\"#bb99ff\"";
		else if (is_bcache_cset(device))
		    out << ", color=\"#6622dd\", fillcolor=\"#bb99ff\"";
		else if (is_mountable(device))
		    out << ", color=\"#008800\", fillcolor=\"#99ee99\"";
		else
		    ST_THROW(LogicException("unknown Device subclass"));

		if (graphviz_flags && GraphvizFlags::TOOLTIP)
		{
		    out << ", tooltip=" << boost::escape_dot_string(tooltip);
		}

		out << " ]";
	    }
	};


	struct write_edge
	{
	    write_edge(const Devicegraph::Impl& devicegraph) : devicegraph(devicegraph) {}

	    const Devicegraph::Impl& devicegraph;

	    void operator()(ostream& out, const Devicegraph::Impl::edge_descriptor& edge) const
	    {
		const Holder* holder = devicegraph[edge];

		if (is_subdevice(holder))
		{
		    out << "[ style=solid ]";
		}
		else if (is_md_user(holder))
		{
		    const MdUser* md_user = to_md_user(holder);
		    if (md_user->is_spare() || md_user->is_faulty())
			out << "[ style=dotted ]";
		    else
			out << "[ style=dashed ]";
		}
		else if (is_user(holder))
		{
		    out << "[ style=dashed ]";
		}
		else
		{
		    ST_THROW(LogicException("unknown Holder subclass"));
		}
	    }
	};

    }


    void
    Devicegraph::Impl::write_graphviz(const string& filename, GraphvizFlags graphviz_flags) const
    {
	ofstream fout(filename);

	fout << "// " << generated_string() << '\n';
	fout << '\n';

	// TODO write same rank stuff, should be possible in write_graph

	// TODO the node must include device name (or better some unique id) to
	// detect clicked objects in YaST

	// TODO in the long run a filesystem must support several mount points, so
	// boost::write_graphviz might not be able to handle our needs (or the
	// needs of YaST).  Just keep a write_graphviz function here for debugging
	// and move the thing YaST needs to yast2-storage.

	VertexIndexMapGenerator<graph_t> vertex_index_map_generator(graph);

	boost::write_graphviz(fout, graph, write_vertex(*this, graphviz_flags), write_edge(*this),
			      write_graph(*this), vertex_index_map_generator.get());

	fout.close();
    }

}
