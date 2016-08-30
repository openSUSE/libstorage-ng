/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    sid_t Device::Impl::global_sid = 42;	// just a random number ;)


    Device::Impl::Impl()
	: sid(global_sid++), devicegraph(nullptr), vertex(), userdata()
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


    void
    Device::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
    }


    void
    Device::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
    }


    void
    Device::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "sid", sid);
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
    Device::Impl::has_children() const
    {
	return devicegraph->get_impl().num_children(vertex) > 0;
    }


    size_t
    Device::Impl::num_children() const
    {
	return devicegraph->get_impl().num_children(vertex);
    }


    bool
    Device::Impl::has_parents() const
    {
	return devicegraph->get_impl().num_parents(vertex) > 0;
    }


    size_t
    Device::Impl::num_parents() const
    {
	return devicegraph->get_impl().num_parents(vertex);
    }


    void
    Device::Impl::remove_descendants()
    {
	Devicegraph::Impl& devicegraph_impl = devicegraph->get_impl();

	for (Devicegraph::Impl::vertex_descriptor descendant : devicegraph_impl.descendants(vertex, false))
	    devicegraph_impl.remove_vertex(descendant);
    }


    ResizeInfo
    Device::Impl::detect_resize_info() const
    {
	return ResizeInfo(false);
    }


    void
    Device::Impl::parent_has_new_region(const Device* parent)
    {
    }


    void
    Device::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(sid));

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
	for (const Device* device : get_device()->get_parents())
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
	    actiongraph.add_vertex(new Action::Reallot(get_sid(), ReallotMode::EXTEND, device));
	}

	for (sid_t sid : removed_sids)
	{
	    const Device* device = actiongraph.get_devicegraph(LHS)->find_device(sid);
	    actiongraph.add_vertex(new Action::Reallot(get_sid(), ReallotMode::REDUCE, device));
	}
    }


    void
    Device::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(sid));

	actiongraph.add_chain(actions);
    }


    bool
    Device::Impl::equal(const Impl& rhs) const
    {
	return sid == rhs.sid;
    }


    void
    Device::Impl::log_diff(std::ostream& log, const Impl& rhs) const
    {
	storage::log_diff(log, "sid", sid, rhs.sid);
    }


    void
    Device::Impl::print(std::ostream& out) const
    {
	out << get_classname() << " sid:" << get_sid()
	    << " displayname:" << get_displayname();
    }


    Text
    Device::Impl::do_create_text(Tense tense) const
    {
	return _("error: stub function called");
    }


    void
    Device::Impl::do_create() const
    {
	throw std::logic_error("stub function called");
    }


    Text
    Device::Impl::do_delete_text(Tense tense) const
    {
	return _("error: stub function called");
    }


    void
    Device::Impl::do_delete() const
    {
	throw std::logic_error("stub function called");
    }


    Text
    Device::Impl::do_resize_text(ResizeMode resize_mode, const Device* lhs, Tense tense) const
    {
	return _("error: stub function called");
    }


    void
    Device::Impl::do_resize(ResizeMode resize_mode) const
    {
	throw std::logic_error("stub function called");
    }


    Text
    Device::Impl::do_reallot_text(ReallotMode reallot_mode, const Device* device, Tense tense) const
    {
	return _("error: stub function called");
    }


    void
    Device::Impl::do_reallot(ReallotMode reallot_mode, const Device* device) const
    {
	throw std::logic_error("stub function called");
    }


    namespace Action
    {

	Text
	Resize::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Device* device_lhs = get_device_lhs(actiongraph);
	    const Device* device_rhs = get_device_rhs(actiongraph);
	    return device_rhs->get_impl().do_resize_text(resize_mode, device_lhs, tense);
	}


	void
	Resize::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Device* device = get_device_rhs(actiongraph);
	    device->get_impl().do_resize(resize_mode);
	}


	Text
	Reallot::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Device* device_rhs = get_device_rhs(actiongraph);
	    return device_rhs->get_impl().do_reallot_text(reallot_mode, device, tense);
	}


	void
	Reallot::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Device* device_rhs = get_device_rhs(actiongraph);
	    device_rhs->get_impl().do_reallot(reallot_mode, device);
	}


	void
	Reallot::add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const
	{
	    Modify::add_dependencies(v, actiongraph);

	    if (reallot_mode == ReallotMode::REDUCE)
	    {
		// Make sure the device (PV) is removed before being destroyed
		for (Actiongraph::Impl::vertex_descriptor tmp :
			 actiongraph.actions_with_sid(device->get_sid(), ONLY_FIRST))
		    actiongraph.add_edge(v, tmp);
	    }
	    else
	    {
		// Make sure the device is created before being added
		for (Actiongraph::Impl::vertex_descriptor tmp :
			 actiongraph.actions_with_sid(device->get_sid(), ONLY_LAST))
		    actiongraph.add_edge(tmp, v);

		// If the device was assigned elsewhere, make sure it's removed
		// from there before being re-assigned
		for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.vertices())
		    if (action_removes_device(actiongraph[tmp]))
		    {
			actiongraph.add_edge(tmp, v);
			break;
		    }
	    }
	}

	bool
	Reallot::action_removes_device(const Action::Base* action) const
	{
	    const Action::Reallot* reallot;

	    reallot = dynamic_cast<const Action::Reallot*>(action);
	    if (!reallot) return false;
	    if (reallot->reallot_mode != ReallotMode::REDUCE) return false;
	    return (reallot->device->get_sid() == device->get_sid());
	}

    }

}
