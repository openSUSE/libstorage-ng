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


#include "storage/Holders/HolderImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    Holder::Impl::Impl()
	: devicegraph(nullptr), userdata()
    {
    }


    Holder::Impl::Impl(const xmlNode* node)
	: devicegraph(nullptr), userdata()
    {
    }


    bool
    Holder::Impl::operator==(const Impl& rhs) const
    {
	if (typeid(*this) != typeid(rhs))
	    return false;

	return equal(rhs);
    }


    Holder*
    Holder::Impl::copy_to_devicegraph(Devicegraph* devicegraph) const
    {
	ST_CHECK_PTR(devicegraph);

	sid_t source_sid = get_source_sid();
	sid_t target_sid = get_target_sid();

	if (!devicegraph->device_exists(source_sid))
	    ST_THROW(Exception(sformat("source device does not exist, source-sid:%d", source_sid)));

	if (!devicegraph->device_exists(target_sid))
	    ST_THROW(Exception(sformat("target device does not exist, target-sid:%d", target_sid)));

	if (devicegraph->holder_exists(source_sid, target_sid))
	    ST_THROW(Exception(sformat("holder already exists, source-sid:%d, target-sid:%d",
				       source_sid, target_sid)));

	Devicegraph::Impl::vertex_descriptor source = devicegraph->get_impl().find_vertex(source_sid);
	Devicegraph::Impl::vertex_descriptor target = devicegraph->get_impl().find_vertex(target_sid);

	shared_ptr<Holder> holder = get_non_impl()->clone_v2();

	Devicegraph::Impl::edge_descriptor edge = devicegraph->get_impl().add_edge_v2(source, target, holder);
	holder->get_impl().set_devicegraph_and_edge(devicegraph, edge);

	return holder.get();
    }


    bool
    Holder::Impl::exists_in_devicegraph(const Devicegraph* devicegraph) const
    {
	ST_CHECK_PTR(devicegraph);

	return devicegraph->holder_exists(get_source_sid(), get_target_sid());
    }


    bool
    Holder::Impl::exists_in_probed() const
    {
	return exists_in_devicegraph(get_storage()->get_probed());
    }


    bool
    Holder::Impl::exists_in_staging() const
    {
	return exists_in_devicegraph(get_storage()->get_staging());
    }


    bool
    Holder::Impl::exists_in_system() const
    {
	return exists_in_devicegraph(get_storage()->get_system());
    }


    Storage*
    Holder::Impl::get_storage()
    {
	return get_devicegraph()->get_storage();
    }


    const Storage*
    Holder::Impl::get_storage() const
    {
	return get_devicegraph()->get_storage();
    }


    void
    Holder::Impl::set_devicegraph_and_edge(Devicegraph* devicegraph,
					   Devicegraph::Impl::edge_descriptor edge)
    {
	Impl::devicegraph = devicegraph;
	Impl::edge = edge;

	const Holder* holder = devicegraph->get_impl()[edge];
	if (&holder->get_impl() != this)
	    ST_THROW(LogicException("wrong edge for back references"));
    }


    void
    Holder::Impl::set_edge(Devicegraph::Impl::edge_descriptor edge)
    {
	Impl::edge = edge;

	const Holder* holder = devicegraph->get_impl()[edge];
	if (&holder->get_impl() != this)
	    ST_THROW(LogicException("wrong edge for back references"));
    }


    Device*
    Holder::Impl::get_source()
    {
	Devicegraph::Impl::vertex_descriptor source = devicegraph->get_impl().source(edge);
	return devicegraph->get_impl()[source];
    }


    const Device*
    Holder::Impl::get_source() const
    {
	Devicegraph::Impl::vertex_descriptor source = devicegraph->get_impl().source(edge);
	return devicegraph->get_impl()[source];
    }


    void
    Holder::Impl::set_source(const Device* source)
    {
	if (get_devicegraph() != source->get_devicegraph())
	    ST_THROW(Exception("new source has wrong devicegraph"));

	Devicegraph::Impl::vertex_descriptor vertex = source->get_impl().get_vertex();
	devicegraph->get_impl().set_source(edge, vertex);
    }


    void
    Holder::Impl::set_target(const Device* target)
    {
	if (get_devicegraph() != target->get_devicegraph())
	    ST_THROW(Exception("new target has wrong devicegraph"));

	Devicegraph::Impl::vertex_descriptor vertex = target->get_impl().get_vertex();
	devicegraph->get_impl().set_target(edge, vertex);
    }


    sid_t
    Holder::Impl::get_source_sid() const
    {
	return get_source()->get_sid();
    }


    Device*
    Holder::Impl::get_target()
    {
	Devicegraph::Impl::vertex_descriptor target = devicegraph->get_impl().target(edge);
	return devicegraph->get_impl()[target];
    }


    const Device*
    Holder::Impl::get_target() const
    {
	Devicegraph::Impl::vertex_descriptor target = devicegraph->get_impl().target(edge);
	return devicegraph->get_impl()[target];
    }


    sid_t
    Holder::Impl::get_target_sid() const
    {
	return get_target()->get_sid();
    }


    void
    Holder::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "source-sid", get_source_sid());
	setChildValue(node, "target-sid", get_target_sid());
    }


    void
    Holder::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
    }


    void
    Holder::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Holder* lhs) const
    {
    }


    void
    Holder::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
    }


    bool
    Holder::Impl::equal(const Impl& rhs) const
    {
	// TODO handle source and target sid here?

	return userdata == rhs.userdata;
    }


    void
    Holder::Impl::log_diff(std::ostream& log, const Impl& rhs) const
    {
	// TODO handle source and target sid here?

	storage::log_diff(log, "userdata", userdata, rhs.userdata);
    }


    void
    Holder::Impl::print(std::ostream& out) const
    {
	out << get_classname() << " source-sid:" << get_source_sid()
	    << " target-sid:" << get_target_sid();

	if (!userdata.empty())
	    out << " userdata:" << userdata;
    }


    Text
    Holder::Impl::do_create_text(Tense tense) const
    {
	return UntranslatedText(sformat("error: stub Holder::Impl::do_create_text called for %s", get_classname()));
    }


    void
    Holder::Impl::do_create()
    {
	ST_THROW(LogicException("stub Holder::Impl::do_create called"));
    }


    Text
    Holder::Impl::do_delete_text(Tense tense) const
    {
	return UntranslatedText(sformat("error: stub Holder::Impl::do_delete_text called for %s", get_classname()));
    }


    void
    Holder::Impl::do_delete() const
    {
	ST_THROW(LogicException("stub Holder::Impl::do_delete called"));
    }


    void
    Holder::Impl::create(Devicegraph* devicegraph, const Device* source, const Device* target,
			  std::shared_ptr<Holder> holder)
    {
	add_to_devicegraph(devicegraph, source, target, holder);
    }


    void
    Holder::Impl::load(Devicegraph* devicegraph, const xmlNode* node, std::shared_ptr<Holder> holder)
    {
	sid_t source_sid = 0;
	if (!getChildValue(node, "source-sid", source_sid))
	    ST_THROW(Exception("no source-sid"));

	sid_t target_sid = 0;
	if (!getChildValue(node, "target-sid", target_sid))
	    ST_THROW(Exception("no target-sid"));

	const Device* source = devicegraph->find_device(source_sid);
	const Device* target = devicegraph->find_device(target_sid);

	add_to_devicegraph(devicegraph, source, target, holder);
    }


    void
    Holder::Impl::add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
				     const Device* target, shared_ptr<Holder> holder)
    {
	ST_CHECK_PTR(devicegraph);
	ST_CHECK_PTR(source);
	ST_CHECK_PTR(target);

	if (source->get_devicegraph() != devicegraph)
	    ST_THROW(Exception("wrong graph in source"));

	if (target->get_devicegraph() != devicegraph)
	    ST_THROW(Exception("wrong graph in target"));

	Devicegraph::Impl::vertex_descriptor source_vertex = source->get_impl().get_vertex();
	Devicegraph::Impl::vertex_descriptor target_vertex = target->get_impl().get_vertex();

	Devicegraph::Impl::edge_descriptor edge = devicegraph->get_impl().add_edge_v2(source_vertex, target_vertex, holder);

	holder->get_impl().set_devicegraph_and_edge(devicegraph, edge);
    }


}
