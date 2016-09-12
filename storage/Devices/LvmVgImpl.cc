/*
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


#include <iostream>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/Math.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/User.h"
#include "storage/FindBy.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<LvmVg>::classname = "LvmVg";


    LvmVg::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), vg_name(), uuid(), region(0, 0, default_extent_size)
    {
	if (!getChildValue(node, "vg-name", vg_name))
	    ST_THROW(Exception("no vg-name"));

	getChildValue(node, "uuid", uuid);

	getChildValue(node, "region", region);
    }


    void
    LvmVg::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "vg-name", vg_name);
	setChildValue(node, "uuid", uuid);

	setChildValue(node, "region", region);
    }


    void
    LvmVg::Impl::probe_lvm_vgs(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const CmdVgs::Vg& vg : systeminfo.getCmdVgs().get_vgs())
	{
	    LvmVg* lvm_vg = LvmVg::create(probed, vg.vg_name);
	    lvm_vg->get_impl().set_uuid(vg.vg_uuid);
	    lvm_vg->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    LvmVg::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Device::Impl::probe_pass_1(probed, systeminfo);

	const CmdVgs& cmd_vgs = systeminfo.getCmdVgs();
	const CmdVgs::Vg& vg = cmd_vgs.find_by_vg_uuid(uuid);

	region = Region(0, vg.extent_count, vg.extent_size);
    }


    unsigned long long
    LvmVg::Impl::get_size() const
    {
	return region.to_bytes(region.get_length());
    }


    string
    LvmVg::Impl::get_size_string() const
    {
	return byte_to_humanstring(get_size(), false, 2, false);
    }


    void
    LvmVg::Impl::set_extent_size(unsigned long long extent_size)
    {
	// see vgcreate(8) for valid values

	if (!is_power_of_two(extent_size) || !is_multiple_of(extent_size, 128 * KiB))
	    ST_THROW(InvalidExtentSize(extent_size));

	region.set_block_size(extent_size);

	// TODO adjust lvm lvs
    }


    unsigned long long
    LvmVg::Impl::number_of_used_extents() const
    {
	unsigned long long ret = 0;

	for (const LvmLv* lvm_lv : get_lvm_lvs())
	    ret += lvm_lv->get_impl().number_of_extents();

	return ret;
    }


    unsigned long long
    LvmVg::Impl::number_of_free_extents() const
    {
	unsigned long long a = number_of_extents();
	unsigned long long b = number_of_used_extents();

	return b >= a ? 0 : a - b;
    }


    void
    LvmVg::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region();
    }


    void
    LvmVg::Impl::set_vg_name(const string& vg_name)
    {
	Impl::vg_name = vg_name;

	// TODO call set_name() for all lvm_lvs
    }


    void
    LvmVg::Impl::calculate_region()
    {
	unsigned long long extent_size = region.get_block_size();

	unsigned long long extent_count = 0;

	for (const LvmPv* lvm_pv : get_lvm_pvs())
	{
	    // TODO 1MiB due to metadata and physical extent alignment, needs more research

	    unsigned long long size = lvm_pv->get_blk_device()->get_size();
	    if (size >= 1 * MiB)
		extent_count += (size - 1 * MiB) / extent_size;
	}

	region.set_length(extent_count);
    }


    LvmPv*
    LvmVg::Impl::add_lvm_pv(BlkDevice* blk_device)
    {
	Devicegraph* devicegraph = get_devicegraph();

	LvmPv* lvm_pv = nullptr;

	switch (blk_device->num_children())
	{
	    case 0:
		lvm_pv = LvmPv::create(devicegraph);
		User::create(devicegraph, blk_device, lvm_pv);
		break;

	    case 1:
		lvm_pv = blk_device->get_impl().get_single_child_of_type<LvmPv>();
		break;

	    default:
		ST_THROW(Exception("illegal number of children"));
	}

	Subdevice::create(devicegraph, lvm_pv, get_device());

	calculate_region();

	return lvm_pv;
    }


    void
    LvmVg::Impl::remove_lvm_pv(BlkDevice* blk_device)
    {
	LvmPv* lvm_pv = blk_device->get_impl().get_single_child_of_type<LvmPv>();

	LvmVg* lvm_vg = lvm_pv->get_impl().get_single_child_of_type<LvmVg>();

	if (lvm_vg != get_device())
	    ST_THROW(Exception("not a blk device of volume group"));

	Devicegraph* devicegraph = get_devicegraph();

	devicegraph->get_impl().remove_vertex(lvm_pv->get_impl().get_vertex());

	calculate_region();
    }


    vector<LvmPv*>
    LvmVg::Impl::get_lvm_pvs()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO sorting

	return devicegraph.filter_devices_of_type<LvmPv>(devicegraph.parents(vertex));
    }


    vector<const LvmPv*>
    LvmVg::Impl::get_lvm_pvs() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO sorting

	return devicegraph.filter_devices_of_type<const LvmPv>(devicegraph.parents(vertex));
    }


    LvmLv*
    LvmVg::Impl::create_lvm_lv(const std::string& lv_name, unsigned long long size)
    {
	Devicegraph* devicegraph = get_devicegraph();

	LvmLv* lvm_lv = LvmLv::create(devicegraph, vg_name, lv_name);
	Subdevice::create(devicegraph, get_device(), lvm_lv);

	unsigned long long extent_size = region.get_block_size();
	lvm_lv->set_region(Region(0, size / extent_size, extent_size));

	return lvm_lv;
    }


    void
    LvmVg::Impl::delete_lvm_lv(LvmLv* lvm_lv)
    {
	lvm_lv->remove_descendants();

	get_devicegraph()->remove_device(lvm_lv);
    }


    LvmLv*
    LvmVg::Impl::get_lvm_lv(const string& lv_name)
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	for (LvmLv* lvm_lv : devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex)))
	{
	    if (lvm_lv->get_lv_name() == lv_name)
		return lvm_lv;
	}

	ST_THROW(Exception("lvm lv not found"));
	__builtin_unreachable();
    }


    vector<LvmLv*>
    LvmVg::Impl::get_lvm_lvs()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex),
							 compare_by_lv_name);
    }


    vector<const LvmLv*>
    LvmVg::Impl::get_lvm_lvs() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex),
							 compare_by_lv_name);
    }


    LvmVg*
    LvmVg::Impl::find_by_uuid(Devicegraph* devicegraph, const std::string& uuid)
    {
	return storage::find_by_uuid<LvmVg>(devicegraph, uuid);
    }


    const LvmVg*
    LvmVg::Impl::find_by_uuid(const Devicegraph* devicegraph, const std::string& uuid)
    {
	return storage::find_by_uuid<const LvmVg>(devicegraph, uuid);
    }


    bool
    LvmVg::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return vg_name == rhs.vg_name && uuid == rhs.uuid && region == rhs.region;
    }


    void
    LvmVg::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "vg-name", vg_name, rhs.vg_name);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "region", region, rhs.region);
    }


    void
    LvmVg::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " vg-name:" << vg_name << " uuid:" << uuid
	    << " region:" << region;
    }


    Text
    LvmVg::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Create volume group %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Creating volume group %1$s (%2$s)"));

	return sformat(text, vg_name.c_str(), get_size_string().c_str());
    }


    void
    LvmVg::Impl::do_create() const
    {
	string cmd_line = VGCREATEBIN " --physicalextentsize " + to_string(get_extent_size()) +
	    "b " + quote(vg_name);

	for (const LvmPv* lvm_pv : get_lvm_pvs())
	    cmd_line += " " + quote(lvm_pv->get_blk_device()->get_name());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create LvmVg failed"));
    }


    Text
    LvmVg::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Delete volume group %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Deleting volume group %1$s (%2$s)"));

	return sformat(text, vg_name.c_str(), get_size_string().c_str());
    }


    void
    LvmVg::Impl::do_delete() const
    {
	string cmd_line = VGREMOVEBIN " " + quote(vg_name);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete LvmVg failed"));
    }


    Text
    LvmVg::Impl::do_reallot_text(ReallotMode reallot_mode, const Device* device, Tense tense) const
    {
	Text text;

	switch (reallot_mode)
	{
	    case ReallotMode::REDUCE:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Remove %1$s from %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Removing %1$s from %2$s"));
		break;

	    case ReallotMode::EXTEND:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Add %1$s to %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Adding %1$s to %2$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for reallot_mode"));
	}

	const LvmPv* lvm_pv = to_lvm_pv(device);
	const BlkDevice* blk_device = lvm_pv->get_blk_device();

	return sformat(text, blk_device->get_name().c_str(), vg_name.c_str());
    }


    void
    LvmVg::Impl::do_reallot(ReallotMode reallot_mode, const Device* device) const
    {
	const LvmPv* lvm_pv = to_lvm_pv(device);

	switch (reallot_mode)
	{
	    case ReallotMode::REDUCE:
		do_reduce(lvm_pv);
		return;

	    case ReallotMode::EXTEND:
		do_extend(lvm_pv);
		return;
	}

	ST_THROW(LogicException("invalid value for reallot_mode"));
    }


    void
    LvmVg::Impl::do_reduce(const LvmPv* lvm_pv) const
    {
	string cmd_line = VGREDUCEBIN " " + quote(vg_name) + " " + quote(lvm_pv->get_blk_device()->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("reduce LvmVg failed"));
    }


    void
    LvmVg::Impl::do_extend(const LvmPv* lvm_pv) const
    {
	string cmd_line = VGEXTENDBIN " " + quote(vg_name) + " " + quote(lvm_pv->get_blk_device()->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("extend LvmVg failed"));
    }


    bool
    compare_by_vg_name(const LvmVg* lhs, const LvmVg* rhs)
    {
	return lhs->get_vg_name() < rhs->get_vg_name();
    }


    void
    LvmVg::Impl::add_dependencies(Actiongraph::Impl& actiongraph) const
    {
	// First, all the operations removing or shrinking LVs
	vector<Actiongraph::Impl::vertex_descriptor> decrease_lv_actions;

	// Afterward, all the operations adding or removing PVs
	vector<Actiongraph::Impl::vertex_descriptor> reallot_actions;

	// Finally, operations adding and growing LVs
	vector<Actiongraph::Impl::vertex_descriptor> increase_lv_actions;

	// Look for the relevant actions
	for (Actiongraph::Impl::vertex_descriptor vertex : actiongraph.vertices())
	{
	    const Action::Base* action = actiongraph[vertex];

	    if (action_is_my_reallot(action, actiongraph))
		reallot_actions.push_back(vertex);
	    else if (action_uses_vg_space(action, actiongraph))
		increase_lv_actions.push_back(vertex);
	    else if (action_frees_vg_space(action, actiongraph))
		decrease_lv_actions.push_back(vertex);
	}

	// Add the dependencies to the action graph
	actiongraph.add_chain({ decrease_lv_actions, reallot_actions, increase_lv_actions });
    }


    bool
    LvmVg::Impl::action_is_my_reallot(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	const Action::Reallot* reallot = dynamic_cast<const Action::Reallot*>(action);
	return reallot && reallot->sid == get_sid();
    }


    bool
    LvmVg::Impl::action_uses_vg_space(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	if (create_action)
	    return is_my_lvm_lv(create_action->get_device_rhs(actiongraph));

	const Action::Resize* resize_action = dynamic_cast<const Action::Resize*>(action);
	if (resize_action && resize_action->resize_mode == ResizeMode::GROW)
	    return is_my_lvm_lv(resize_action->get_device_rhs(actiongraph));

	return false;
    }


    bool
    LvmVg::Impl::action_frees_vg_space(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
	if (delete_action)
	    return is_my_lvm_lv(delete_action->get_device_lhs(actiongraph));

	const Action::Resize* resize_action = dynamic_cast<const Action::Resize*>(action);
	if (resize_action && resize_action->resize_mode == ResizeMode::SHRINK)
	    return is_my_lvm_lv(resize_action->get_device_lhs(actiongraph));

	return false;
    }


    bool
    LvmVg::Impl::is_my_lvm_lv(const Device* device) const
    {
	if (!is_lvm_lv(device))
	    return false;

	const LvmLv* lvm_lv = to_lvm_lv(device);
	return lvm_lv->get_lvm_vg()->get_sid() == get_sid();
    }

}
