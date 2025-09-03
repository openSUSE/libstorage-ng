/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Format.h"


namespace storage
{

    HolderHasWrongType::HolderHasWrongType(const char* seen, const char* expected)
	: Exception(sformat("holder has wrong type, seen '%s', expected '%s'", seen, expected),
		    Silencer::is_any_active() ? LogLevel::DEBUG : LogLevel::WARNING)
    {
    }


    Holder::Holder(Impl* impl)
	: impl(impl)
    {
	ST_CHECK_PTR(impl);
    }


    Holder::Holder(unique_ptr<Impl>&& impl)
	: impl(std::move(impl))
    {
	ST_CHECK_PTR(Holder::impl);
    }


    Holder::~Holder() = default;


    bool
    Holder::operator==(const Holder& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Holder::operator!=(const Holder& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    Holder*
    Holder::copy_to_devicegraph(Devicegraph* devicegraph) const
    {
	return get_impl().copy_to_devicegraph(devicegraph);
    }


    bool
    Holder::exists_in_devicegraph(const Devicegraph* devicegraph) const
    {
	return get_impl().exists_in_devicegraph(devicegraph);
    }


    bool
    Holder::exists_in_probed() const
    {
	return get_impl().exists_in_probed();
    }


    bool
    Holder::exists_in_staging() const
    {
	return get_impl().exists_in_staging();
    }


    bool
    Holder::exists_in_system() const
    {
	return get_impl().exists_in_system();
    }


    void
    Holder::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	add_to_devicegraph(devicegraph, source, target);
    }


    void
    Holder::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	sid_t source_sid = 0;
	if (!getChildValue(node, "source-sid", source_sid))
	    ST_THROW(Exception("no source-sid"));

	sid_t target_sid = 0;
	if (!getChildValue(node, "target-sid", target_sid))
	    ST_THROW(Exception("no target-sid"));

	const Device* source = devicegraph->find_device(source_sid);
	const Device* target = devicegraph->find_device(target_sid);

	add_to_devicegraph(devicegraph, source, target);
    }


    Device*
    Holder::get_source()
    {
	return get_impl().get_source();
    }


    const Device*
    Holder::get_source() const
    {
	return get_impl().get_source();
    }


    void
    Holder::set_source(const Device* source)
    {
	get_impl().set_source(source);
    }


    void
    Holder::set_target(const Device* target)
    {
	get_impl().set_target(target);
    }


    sid_t
    Holder::get_source_sid() const
    {
	return get_impl().get_source_sid();
    }


    Device*
    Holder::get_target()
    {
	return get_impl().get_target();
    }


    const Device*
    Holder::get_target() const
    {
	return get_impl().get_target();
    }


    sid_t
    Holder::get_target_sid() const
    {
	return get_impl().get_target_sid();
    }


    void
    Holder::add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
			       const Device* target)
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

	Devicegraph::Impl::edge_descriptor edge = devicegraph->get_impl().add_edge(source_vertex, target_vertex, this);

	get_impl().set_devicegraph_and_edge(devicegraph, edge);
    }


    void
    Holder::save(xmlNode* node) const
    {
	get_impl().save(node);
    }


    const map<string, string>&
    Holder::get_userdata() const
    {
	return get_impl().get_userdata();
    }


    void
    Holder::set_userdata(const map<string, string>& userdata)
    {
	get_impl().set_userdata(userdata);
    }


    std::ostream&
    operator<<(std::ostream& out, const Holder& holder)
    {
	holder.get_impl().print(out);
	return out;
    }


    Devicegraph*
    Holder::get_devicegraph()
    {
	return get_impl().get_devicegraph();
    }


    const Devicegraph*
    Holder::get_devicegraph() const
    {
	return get_impl().get_devicegraph();
    }

}
