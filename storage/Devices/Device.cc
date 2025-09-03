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


#include "storage/Devices/DeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/FreeInfo.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    DeviceHasWrongType::DeviceHasWrongType(const char* seen, const char* expected)
	: Exception(sformat("device has wrong type, seen '%s', expected '%s'", seen, expected),
		    Silencer::is_any_active() ? LogLevel::DEBUG : LogLevel::WARNING)
    {
    }


    Device::Device(Impl* impl)
	: impl(impl)
    {
	ST_CHECK_PTR(impl);
    }


    Device::Device(unique_ptr<Impl>&& impl)
	: impl(std::move(impl))
    {
	ST_CHECK_PTR(Device::impl);
    }


    Device::~Device() = default;


    string
    Device::get_displayname() const
    {
	return get_impl().get_displayname();
    }


    bool
    Device::operator==(const Device& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Device::operator!=(const Device& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    Device*
    Device::copy_to_devicegraph(Devicegraph* devicegraph) const
    {
	return get_impl().copy_to_devicegraph(devicegraph);
    }


    bool
    Device::exists_in_devicegraph(const Devicegraph* devicegraph) const
    {
	return get_impl().exists_in_devicegraph(devicegraph);
    }


    bool
    Device::exists_in_probed() const
    {
	return get_impl().exists_in_probed();
    }


    bool
    Device::exists_in_staging() const
    {
	return get_impl().exists_in_staging();
    }


    bool
    Device::exists_in_system() const
    {
	return get_impl().exists_in_system();
    }


    void
    Device::create(Devicegraph* devicegraph)
    {
	add_to_devicegraph(devicegraph);
    }


    void
    Device::load(Devicegraph* devicegraph)
    {
	add_to_devicegraph(devicegraph);
    }


    void
    Device::add_to_devicegraph(Devicegraph* devicegraph)
    {
	ST_CHECK_PTR(devicegraph);

	Devicegraph::Impl::vertex_descriptor vertex = devicegraph->get_impl().add_vertex(this);

	get_impl().set_devicegraph_and_vertex(devicegraph, vertex);
    }


    void
    Device::save(xmlNode* node) const
    {
	get_impl().save(node);
    }


    sid_t
    Device::get_sid() const
    {
	return get_impl().get_sid();
    }


    ResizeInfo
    Device::detect_resize_info() const
    {
	return get_impl().detect_resize_info();
    }


    vector<Device*>
    Device::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Device>();
    }


    vector<const Device*>
    Device::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Device>();
    }


    bool
    Device::has_children() const
    {
	return get_impl().has_children();
    }


    size_t
    Device::num_children() const
    {
	return get_impl().num_children();
    }


    bool
    Device::has_parents() const
    {
	return get_impl().has_parents();
    }


    size_t
    Device::num_parents() const
    {
	return get_impl().num_parents();
    }


    vector<Device*>
    Device::get_children()
    {
	return get_children(View::CLASSIC);
    }


    vector<const Device*>
    Device::get_children() const
    {
	return get_children(View::CLASSIC);
    }


    vector<Device*>
    Device::get_children(View view)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().children(vertex, view));
    }


    vector<const Device*>
    Device::get_children(View view) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().children(vertex, view));
    }


    vector<Device*>
    Device::get_parents()
    {
	return get_parents(View::CLASSIC);
    }


    vector<const Device*>
    Device::get_parents() const
    {
	return get_parents(View::CLASSIC);
    }


    vector<Device*>
    Device::get_parents(View view)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().parents(vertex, view));
    }


    vector<const Device*>
    Device::get_parents(View view) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().parents(vertex, view));
    }


    vector<Device*>
    Device::get_siblings(bool itself)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().siblings(vertex, itself));
    }


    vector<const Device*>
    Device::get_siblings(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().siblings(vertex, itself));
    }


    vector<Device*>
    Device::get_descendants(bool itself)
    {
	return get_descendants(itself, View::CLASSIC);
    }


    vector<const Device*>
    Device::get_descendants(bool itself) const
    {
	return get_descendants(itself, View::CLASSIC);
    }


    vector<Device*>
    Device::get_descendants(bool itself, View view)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().descendants(vertex, itself, view));
    }


    vector<const Device*>
    Device::get_descendants(bool itself, View view) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().descendants(vertex, itself, view));
    }


    vector<Device*>
    Device::get_ancestors(bool itself)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().ancestors(vertex, itself));
    }


    vector<const Device*>
    Device::get_ancestors(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().ancestors(vertex, itself));
    }


    vector<Device*>
    Device::get_leaves(bool itself)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().leaves(vertex, itself));
    }


    vector<const Device*>
    Device::get_leaves(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().leaves(vertex, itself));
    }


    vector<Device*>
    Device::get_roots(bool itself)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().roots(vertex, itself));
    }


    vector<const Device*>
    Device::get_roots(bool itself) const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<Device>(devicegraph->get_impl().roots(vertex, itself));
    }


    vector<Holder*>
    Device::get_in_holders()
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_holders_of_type<Holder>(devicegraph->get_impl().in_edges(vertex));
    }


    vector<const Holder*>
    Device::get_in_holders() const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_holders_of_type<Holder>(devicegraph->get_impl().in_edges(vertex));
    }


    vector<Holder*>
    Device::get_out_holders()
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_holders_of_type<Holder>(devicegraph->get_impl().out_edges(vertex));
    }


    vector<const Holder*>
    Device::get_out_holders() const
    {
	const Devicegraph* devicegraph = get_impl().get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_impl().get_vertex();

	return devicegraph->get_impl().filter_holders_of_type<Holder>(devicegraph->get_impl().out_edges(vertex));
    }


    void
    Device::remove_descendants()
    {
	get_impl().remove_descendants();
    }


    void
    Device::remove_descendants(View view)
    {
	get_impl().remove_descendants(view);
    }


    const map<string, string>&
    Device::get_userdata() const
    {
	return get_impl().get_userdata();
    }


    void
    Device::set_userdata(const map<string, string>& userdata)
    {
	get_impl().set_userdata(userdata);
    }


    bool
    Device::compare_by_sid(const Device* lhs, const Device* rhs)
    {
	return lhs->get_sid() < rhs->get_sid();
    }


    bool
    Device::compare_by_name(const Device* lhs, const Device* rhs)
    {
	return lhs->get_name_sort_key() < rhs->get_name_sort_key();
    }


    string
    Device::get_name_sort_key() const
    {
	return get_impl().get_name_sort_key();
    }


    std::ostream&
    operator<<(std::ostream& out, const Device& device)
    {
	device.get_impl().print(out);
	return out;
    }


    Devicegraph*
    Device::get_devicegraph()
    {
	return get_impl().get_devicegraph();
    }


    const Devicegraph*
    Device::get_devicegraph() const
    {
	return get_impl().get_devicegraph();
    }

}
