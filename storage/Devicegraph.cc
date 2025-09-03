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

#include "storage/Devicegraph.h"
#include "storage/Utils/GraphUtils.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Filesystems/Filesystem.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Holders/HolderImpl.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/Format.h"
#include "storage/GraphvizImpl.h"
#include "storage/EnvironmentImpl.h"


namespace storage
{


    DeviceNotFound::DeviceNotFound(const string& msg)
	: Exception(msg)
    {
    }


    DeviceNotFoundBySid::DeviceNotFoundBySid(sid_t sid)
	: DeviceNotFound(sformat("device not found, sid:%d", sid))
    {
    }


    DeviceNotFoundByName::DeviceNotFoundByName(const string& name)
	: DeviceNotFound(sformat("device not found, name:%s", name))
    {
    }


    DeviceNotFoundByUuid::DeviceNotFoundByUuid(const string& uuid)
	: DeviceNotFound(sformat("device not found, uuid:%s", uuid))
    {
    }


    HolderNotFound::HolderNotFound(const string& msg)
	: Exception(msg)
    {
    }


    HolderNotFoundBySids::HolderNotFoundBySids(sid_t source_sid, sid_t target_sid)
	: HolderNotFound(sformat("holder not found, source_sid:%d, target_sid:%d", source_sid, target_sid))
    {
    }


    HolderAlreadyExists::HolderAlreadyExists(sid_t source_sid, sid_t target_sid)
	: Exception(sformat("holder already exists, source_sid:%d, target_sid:%d", source_sid, target_sid))
    {
    }


    WrongNumberOfParents::WrongNumberOfParents(size_t seen, size_t expected)
	: Exception(sformat("wrong number of parents, seen '%zu', expected '%zu'", seen, expected),
		    Silencer::is_any_active() ? LogLevel::DEBUG : LogLevel::WARNING)
    {
    }


    WrongNumberOfChildren::WrongNumberOfChildren(size_t seen, size_t expected)
	: Exception(sformat("wrong number of children, seen '%zu', expected '%zu'", seen, expected),
		    Silencer::is_any_active() ? LogLevel::DEBUG : LogLevel::WARNING)
    {
    }


    WrongNumberOfHolders::WrongNumberOfHolders(size_t seen, size_t expected)
	: Exception(sformat("wrong number of holders, seen '%zu', expected '%zu'", seen, expected),
		    Silencer::is_any_active() ? LogLevel::DEBUG : LogLevel::WARNING)
    {
    }


    Devicegraph::Devicegraph(Storage* storage)
	: impl(make_unique<Impl>(storage))
    {
	ST_CHECK_PTR(storage);
    }


    Devicegraph::~Devicegraph() = default;


    bool
    Devicegraph::operator==(const Devicegraph& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Devicegraph::operator!=(const Devicegraph& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    Storage*
    Devicegraph::get_storage()
    {
	return get_impl().get_storage();
    }


    const Storage*
    Devicegraph::get_storage() const
    {
	return get_impl().get_storage();
    }


    void
    Devicegraph::load(const string& filename)
    {
	get_impl().load(this, filename, true);
    }


    void
    Devicegraph::load(const string& filename, bool keep_sids)
    {
	get_impl().load(this, filename, keep_sids);
    }


    void
    Devicegraph::save(const string& filename) const
    {
	get_impl().save(filename);
    }


    bool
    Devicegraph::empty() const
    {
	return get_impl().empty();
    }


    size_t
    Devicegraph::num_devices() const
    {
	return get_impl().num_devices();
    }


    size_t
    Devicegraph::num_holders() const
    {
	return get_impl().num_holders();
    }


    std::vector<Disk*>
    Devicegraph::get_all_disks()
    {
	return Disk::get_all(this);
    }


    std::vector<const Disk*>
    Devicegraph::get_all_disks() const
    {
	return Disk::get_all(this);
    }


    std::vector<Md*>
    Devicegraph::get_all_mds()
    {
	return Md::get_all(this);
    }


    std::vector<const Md*>
    Devicegraph::get_all_mds() const
    {
	return Md::get_all(this);
    }


    std::vector<LvmVg*>
    Devicegraph::get_all_lvm_vgs()
    {
	return LvmVg::get_all(this);
    }


    std::vector<const LvmVg*>
    Devicegraph::get_all_lvm_vgs() const
    {
	return LvmVg::get_all(this);
    }


    std::vector<Filesystem*>
    Devicegraph::get_all_filesystems()
    {
	return Filesystem::get_all(this);
    }


    std::vector<const Filesystem*>
    Devicegraph::get_all_filesystems() const
    {
	return Filesystem::get_all(this);
    }


    std::vector<BlkFilesystem*>
    Devicegraph::get_all_blk_filesystems()
    {
	return BlkFilesystem::get_all(this);
    }


    std::vector<const BlkFilesystem*>
    Devicegraph::get_all_blk_filesystems() const
    {
	return BlkFilesystem::get_all(this);
    }


    class CloneCopier
    {

    public:

	CloneCopier(const Devicegraph& g_in, Devicegraph& g_out)
	    : g_in(g_in), g_out(g_out) {}

	void operator()(const Devicegraph::Impl::vertex_descriptor& v_in,
			Devicegraph::Impl::vertex_descriptor& v_out)
	{
	    shared_ptr<Device> device = g_in.get_impl().graph[v_in]->clone_v2();
	    g_out.get_impl().graph[v_out] = device;

	    Device* d_out = g_out.get_impl().graph[v_out].get();
	    d_out->get_impl().set_devicegraph_and_vertex(&g_out, v_out);
	}

	void operator()(const Devicegraph::Impl::edge_descriptor& e_in,
			Devicegraph::Impl::edge_descriptor& e_out)
	{
	    shared_ptr<Holder> holder = g_in.get_impl().graph[e_in]->clone_v2();
	    g_out.get_impl().graph[e_out] = holder;

	    Holder* h_out = g_out.get_impl().graph[e_out].get();
	    h_out->get_impl().set_devicegraph_and_edge(&g_out, e_out);
	}

    private:

	const Devicegraph& g_in;
	Devicegraph& g_out;

    };


    Device*
    Devicegraph::find_device(sid_t sid)
    {
	Impl::vertex_descriptor vertex = get_impl().find_vertex(sid);
	return get_impl()[vertex];
    }


    const Device*
    Devicegraph::find_device(sid_t sid) const
    {
	Impl::vertex_descriptor vertex = get_impl().find_vertex(sid);
	return get_impl()[vertex];
    }


    bool
    Devicegraph::device_exists(sid_t sid) const
    {
	return get_impl().device_exists(sid);
    }


    bool
    Devicegraph::holder_exists(sid_t source_sid, sid_t target_sid) const
    {
	return get_impl().holder_exists(source_sid, target_sid);
    }


    Holder*
    Devicegraph::find_holder(sid_t source_sid, sid_t target_sid)
    {
	Impl::edge_descriptor edge = get_impl().find_edge(source_sid, target_sid);
	return get_impl()[edge];
    }


    const Holder*
    Devicegraph::find_holder(sid_t source_sid, sid_t target_sid) const
    {
	Impl::edge_descriptor edge = get_impl().find_edge(source_sid, target_sid);
	return get_impl()[edge];
    }


    vector<Holder*>
    Devicegraph::find_holders(sid_t source_sid, sid_t target_sid)
    {
	vector<Holder*> ret;

	for (Impl::edge_descriptor edge : get_impl().find_edges(source_sid, target_sid))
	    ret.push_back(get_impl()[edge]);

	return ret;
    }


    vector<const Holder*>
    Devicegraph::find_holders(sid_t source_sid, sid_t target_sid) const
    {
	vector<const Holder*> ret;

	for (Impl::edge_descriptor edge : get_impl().find_edges(source_sid, target_sid))
	    ret.push_back(get_impl()[edge]);

	return ret;
    }


    void
    Devicegraph::clear()
    {
	get_impl().clear();
    }


    void
    Devicegraph::remove_device(sid_t sid)
    {
	Device* device = find_device(sid);
	remove_device(device);
    }


    void
    Devicegraph::remove_device(Device* device)
    {
	get_impl().remove_vertex(device->get_impl().get_vertex());
        // ensure that bcache numbers are correct
	Bcache::reassign_numbers(this);
    }


    void
    Devicegraph::remove_devices(std::vector<Device*> devices)
    {
	for (Device* device : devices)
	    get_impl().remove_vertex(device->get_impl().get_vertex());
	// ensure that bcache numbers are correct
	Bcache::reassign_numbers(this);
    }


    void
    Devicegraph::remove_holder(Holder* holder)
    {
	get_impl().remove_edge(holder->get_impl().get_edge());
    }


    void
    Devicegraph::check(const CheckCallbacks* check_callbacks) const
    {
	get_impl().check(check_callbacks);
    }


    uint64_t
    Devicegraph::used_features() const
    {
	return get_impl().used_features(UsedFeaturesDependencyType::SUGGESTED);
    }


    uf_t
    Devicegraph::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return get_impl().used_features(used_features_dependency_type);
    }


    void
    Devicegraph::copy(Devicegraph& dest) const
    {
	dest.get_impl().clear();

	VertexIndexMapGenerator<Impl::graph_t> vertex_index_map_generator(get_impl().graph);

	CloneCopier copier(*this, dest);

	boost::copy_graph(get_impl().graph, dest.get_impl().graph,
			  vertex_index_map(vertex_index_map_generator.get()).
			  vertex_copy(copier).edge_copy(copier));
    }


    std::ostream&
    operator<<(std::ostream& out, const Devicegraph& devicegraph)
    {
	devicegraph.get_impl().print(out);
	return out;
    }


    void
    Devicegraph::write_graphviz(const string& filename, DevicegraphStyleCallbacks* style_callbacks, View view) const
    {
	if (developer_mode())
	{
	    get_impl().write_graphviz(filename, get_debug_devicegraph_style_callbacks(), View::ALL);
	    return;
	}

	get_impl().write_graphviz(filename, style_callbacks, view);
    }


    void
    Devicegraph::write_graphviz(const string& filename, DevicegraphStyleCallbacks* style_callbacks) const
    {
	if (developer_mode())
	{
	    get_impl().write_graphviz(filename, get_debug_devicegraph_style_callbacks(), View::ALL);
	    return;
	}

	get_impl().write_graphviz(filename, style_callbacks);
    }


    void
    Devicegraph::write_graphviz(const string& filename, GraphvizFlags flags,
				GraphvizFlags tooltip_flags) const
    {
	if (developer_mode())
	{
	    get_impl().write_graphviz(filename, get_debug_devicegraph_style_callbacks(), View::ALL);
	    return;
	}

	AdvancedDevicegraphStyleCallbacks style_callbacks(flags, tooltip_flags);

	get_impl().write_graphviz(filename, &style_callbacks);
    }

}
