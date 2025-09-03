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


#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devicegraph.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/FreeInfo.h"
#include "storage/StorageImpl.h"
#include "storage/Utils/Format.h"
#include "storage/Actions/CreateImpl.h"
#include "storage/Actions/ReallotImpl.h"
#include "storage/Actions/DeleteImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Device>::classname = "Device";


    Device::Impl::Impl()
	: sid(Storage::Impl::get_next_sid()), devicegraph(nullptr), vertex(), userdata()
    {
    }


    Device::Impl::Impl(const xmlNode* node)
	: sid(0), devicegraph(nullptr), vertex(), userdata()
    {
	if (!getChildValue(node, "sid", sid))
	    ST_THROW(Exception("no sid"));
    }


    bool
    Device::Impl::operator==(const Impl& rhs) const
    {
	if (typeid(*this) != typeid(rhs))
	    return false;

	return equal(rhs);
    }


    Device*
    Device::Impl::copy_to_devicegraph(Devicegraph* devicegraph) const
    {
	ST_CHECK_PTR(devicegraph);

	if (exists_in_devicegraph(devicegraph))
	    ST_THROW(Exception(sformat("device already exists, sid:%d", get_sid())));

	shared_ptr<Device> device = get_non_impl()->clone_v2();

	Devicegraph::Impl::vertex_descriptor vertex = devicegraph->get_impl().add_vertex_v2(device);
	device->get_impl().set_devicegraph_and_vertex(devicegraph, vertex);

	return device.get();
    }


    bool
    Device::Impl::exists_in_devicegraph(const Devicegraph* devicegraph) const
    {
	ST_CHECK_PTR(devicegraph);

	return devicegraph->device_exists(sid);
    }


    bool
    Device::Impl::exists_in_probed() const
    {
	return exists_in_devicegraph(get_storage()->get_probed());
    }


    bool
    Device::Impl::exists_in_staging() const
    {
	return exists_in_devicegraph(get_storage()->get_staging());
    }


    bool
    Device::Impl::exists_in_system() const
    {
	return exists_in_devicegraph(get_storage()->get_system());
    }


    Storage*
    Device::Impl::get_storage()
    {
	return get_devicegraph()->get_storage();
    }


    const Storage*
    Device::Impl::get_storage() const
    {
	return get_devicegraph()->get_storage();
    }


    void
    Device::Impl::probe_pass_1a(Prober& prober)
    {
    }


    void
    Device::Impl::probe_pass_1b(Prober& prober)
    {
    }


    void
    Device::Impl::probe_pass_1c(Prober& prober)
    {
    }


    void
    Device::Impl::probe_pass_1f(Prober& prober)
    {
    }


    void
    Device::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "sid", sid);
    }


    void
    Device::Impl::check(const CheckCallbacks* check_callbacks) const
    {
    }


    void
    Device::Impl::set_devicegraph_and_vertex(Devicegraph* devicegraph,
					     Devicegraph::Impl::vertex_descriptor vertex)
    {
	Impl::devicegraph = devicegraph;
	Impl::vertex = vertex;

	const Device* device = devicegraph->get_impl()[vertex];
	if (&device->get_impl() != this)
	    ST_THROW(LogicException("wrong vertex for back references"));
    }


    Devicegraph*
    Device::Impl::get_devicegraph()
    {
	if (!devicegraph)
	    ST_THROW(LogicException("not part of a devicegraph"));

	return devicegraph;
    }


    const Devicegraph*
    Device::Impl::get_devicegraph() const
    {
	if (!devicegraph)
	    ST_THROW(LogicException("not part of a devicegraph"));

	return devicegraph;
    }


    Devicegraph::Impl::vertex_descriptor
    Device::Impl::get_vertex() const
    {
	if (!devicegraph)
	    ST_THROW(LogicException("not part of a devicegraph"));

	return vertex;
    }


    bool
    Device::Impl::has_children(View view) const
    {
	return devicegraph->get_impl().num_children(vertex, view) != 0;
    }


    size_t
    Device::Impl::num_children(View view) const
    {
	return devicegraph->get_impl().num_children(vertex, view);
    }


    bool
    Device::Impl::has_parents(View view) const
    {
	return devicegraph->get_impl().num_parents(vertex, view) != 0;
    }


    size_t
    Device::Impl::num_parents(View view) const
    {
	return devicegraph->get_impl().num_parents(vertex, view);
    }


    void
    Device::Impl::remove_descendants(View view)
    {
	Devicegraph::Impl& devicegraph_impl = devicegraph->get_impl();

	for (Devicegraph::Impl::vertex_descriptor descendant : devicegraph_impl.descendants(vertex, false, view))
	    devicegraph_impl.remove_vertex(descendant);
    }


    bool
    Device::Impl::has_any_active_descendants() const
    {
	for (const Device* descendant : get_non_impl()->get_descendants(false))
	{
	    if (is_mount_point(descendant) && to_mount_point(descendant)->is_active())
		return true;

	    if (is_blk_device(descendant) && to_blk_device(descendant)->is_active())
		return true;
	}

	return false;
    }


    void
    Device::Impl::has_new_parent()
    {
    }


    void
    Device::Impl::parent_has_new_region(const Device* parent)
    {
    }


    void
    Device::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Create>(sid));

	actiongraph.add_chain(actions);
    }


    void
    Device::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const
    {
	add_reallot_actions(actiongraph, lhs);
    }


    void
    Device::Impl::add_reallot_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const
    {
	set<sid_t> lhs_sids;
	for (const Device* device : lhs->get_parents())
	    lhs_sids.insert(device->get_sid());

	set<sid_t> rhs_sids;
	for (const Device* device : get_non_impl()->get_parents())
	    rhs_sids.insert(device->get_sid());

	vector<sid_t> added_sids;
	set_difference(rhs_sids.begin(), rhs_sids.end(), lhs_sids.begin(), lhs_sids.end(),
		       back_inserter(added_sids));

	vector<sid_t> removed_sids;
	set_difference(lhs_sids.begin(), lhs_sids.end(), rhs_sids.begin(), rhs_sids.end(),
		       back_inserter(removed_sids));

	for (sid_t sid : added_sids)
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);
	    actiongraph.add_vertex(make_shared<Action::Reallot>(get_sid(), ReallotMode::EXTEND, device));
	}

	for (sid_t sid : removed_sids)
	{
	    const Device* device = actiongraph.get_devicegraph(LHS)->find_device(sid);
	    actiongraph.add_vertex(make_shared<Action::Reallot>(get_sid(), ReallotMode::REDUCE, device));
	}
    }


    void
    Device::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Delete>(sid));

	actiongraph.add_chain(actions);
    }


    bool
    Device::Impl::equal(const Impl& rhs) const
    {
	return sid == rhs.sid && userdata == rhs.userdata;
    }


    void
    Device::Impl::log_diff(std::ostream& log, const Impl& rhs) const
    {
	storage::log_diff(log, "sid", sid, rhs.sid);

	storage::log_diff(log, "userdata", userdata, rhs.userdata);
    }


    void
    Device::Impl::print(std::ostream& out) const
    {
	out << get_classname() << " sid:" << get_sid()
	    << " displayname:" << get_displayname();

	if (!userdata.empty())
	    out << " userdata:" << userdata;
    }


    Text
    Device::Impl::do_create_text(Tense tense) const
    {
	return UntranslatedText(sformat("error: stub Device::Impl::do_create_text called for %s", get_classname()));
    }


    void
    Device::Impl::do_create()
    {
	ST_THROW(LogicException("stub Device::Impl::do_create called"));
    }


    void
    Device::Impl::do_create_post_verify() const
    {
    }


    Text
    Device::Impl::do_delete_text(Tense tense) const
    {
	return UntranslatedText(sformat("error: stub Device::Impl::do_delete_text called for %s", get_classname()));
    }


    void
    Device::Impl::do_delete() const
    {
	ST_THROW(LogicException("stub Device::Impl::do_delete called"));
    }


    Text
    Device::Impl::do_activate_text(Tense tense) const
    {
	return UntranslatedText(sformat("error: stub Device::Impl::do_activate_text called for %s", get_classname()));
    }


    void
    Device::Impl::do_activate()
    {
	ST_THROW(LogicException("stub Device::Impl::do_activate called"));
    }


    void
    Device::Impl::do_activate_post_verify() const
    {
    }


    Text
    Device::Impl::do_deactivate_text(Tense tense) const
    {
	return UntranslatedText(sformat("error: stub Device::Impl::do_deactivate_text called for %s", get_classname()));
    }


    void
    Device::Impl::do_deactivate()
    {
	ST_THROW(LogicException("stub Device::Impl::do_deactivate called"));
    }


    Text
    Device::Impl::do_resize_text(const CommitData& commit_data, const Action::Resize* action) const
    {
	return UntranslatedText(sformat("error: stub Device::Impl::do_resize_text called for %s", get_classname()));
    }


    void
    Device::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	ST_THROW(LogicException("stub Device::Impl::do_resize called"));
    }


    Text
    Device::Impl::do_reallot_text(const CommitData& commit_data, const Action::Reallot* action) const
    {
	return UntranslatedText(sformat("error: stub Device::Impl::do_reallot_text called for %s", get_classname()));
    }


    void
    Device::Impl::do_reallot(const CommitData& commit_data, const Action::Reallot* action) const
    {
	ST_THROW(LogicException("stub Device::Impl::do_reallot called"));
    }


    void
    Device::Impl::create(Devicegraph* devicegraph, shared_ptr<Device> device)
    {
	add_to_devicegraph(devicegraph, device);
    }


    void
    Device::Impl::load(Devicegraph* devicegraph, shared_ptr<Device> device)
    {
	add_to_devicegraph(devicegraph, device);
    }


    void
    Device::Impl::add_to_devicegraph(Devicegraph* devicegraph, shared_ptr<Device> device)
    {
	ST_CHECK_PTR(devicegraph);

	Devicegraph::Impl::vertex_descriptor vertex = devicegraph->get_impl().add_vertex_v2(device);

	device->get_impl().set_devicegraph_and_vertex(devicegraph, vertex);
    }

}
