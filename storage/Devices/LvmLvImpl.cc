/*
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


#include <iostream>

#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/FreeInfo.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/FindBy.h"


using namespace std;


namespace storage
{

    const char* DeviceTraits<LvmLv>::classname = "LvmLv";


    LvmLv::Impl::Impl(const string& vg_name, const string& lv_name)
	: BlkDevice::Impl(make_name(vg_name, lv_name)), lv_name(lv_name), uuid(), stripes(0),
	  stripe_size(0)
    {
	set_dm_table_name(make_dm_table_name(vg_name, lv_name));
    }


    LvmLv::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), lv_name(), uuid(), stripes(0), stripe_size(0)
    {
	if (get_dm_table_name().empty())
	    ST_THROW(Exception("no dm-table-name"));

	if (!getChildValue(node, "lv-name", lv_name))
	    ST_THROW(Exception("no lv-name"));

	getChildValue(node, "uuid", uuid);

	getChildValue(node, "stripes", stripes);
	getChildValue(node, "stripe-size", stripe_size);
    }


    void
    LvmLv::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "lv-name", lv_name);
	setChildValue(node, "uuid", uuid);

	setChildValueIf(node, "stripes", stripes, stripes != 0);
	setChildValueIf(node, "stripe-size", stripe_size, stripe_size != 0);
    }


    void
    LvmLv::Impl::check() const
    {
	BlkDevice::Impl::check();

	if (get_lv_name().empty())
	    ST_THROW(Exception("LvmLv has no lv-name"));

	if (get_region().get_start() != 0)
	    ST_THROW(Exception("LvmLv region start not zero"));
    }


    void
    LvmLv::Impl::probe_lvm_lvs(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const CmdLvs::Lv& lv : systeminfo.getCmdLvs().get_lvs())
	{
	    LvmVg* lvm_vg = LvmVg::Impl::find_by_uuid(probed, lv.vg_uuid);
	    LvmLv* lvm_lv = lvm_vg->create_lvm_lv(lv.lv_name, lv.size);
	    lvm_lv->get_impl().set_uuid(lv.lv_uuid);
	    lvm_lv->get_impl().set_active(lv.active);
	    lvm_lv->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    LvmLv::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_1(probed, systeminfo);

	const LvmVg* lvm_vg = get_lvm_vg();

	set_dm_table_name(make_dm_table_name(lvm_vg->get_vg_name(), lv_name));

	if (is_active())
	{
	    const CmdDmsetupTable& cmd_dmsetup_table = systeminfo.getCmdDmsetupTable();
	    vector<CmdDmsetupTable::Table> tables = cmd_dmsetup_table.get_tables(get_dm_table_name());
	    if (tables[0].target == "striped")
	    {
		stripes = tables[0].stripes;
		stripe_size = tables[0].stripe_size;
	    }
	}
    }


    void
    LvmLv::Impl::set_lv_name(const string& lv_name)
    {
	Impl::lv_name = lv_name;

	const LvmVg* lvm_vg = get_lvm_vg();
	set_name(make_name(lvm_vg->get_vg_name(), lv_name));
	set_dm_table_name(make_dm_table_name(lvm_vg->get_vg_name(), lv_name));

	// TODO clear or update udev-ids; update looks difficult/impossible.
    }


    const LvmVg*
    LvmLv::Impl::get_lvm_vg() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_lvm_vg(get_devicegraph()->get_impl()[vertex]);
    }


    LvmLv*
    LvmLv::Impl::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<LvmLv>(devicegraph, uuid);
    }


    const LvmLv*
    LvmLv::Impl::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const LvmLv>(devicegraph, uuid);
    }


    bool
    LvmLv::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return lv_name == rhs.lv_name && uuid == rhs.uuid && stripes == rhs.stripes &&
	    stripe_size == rhs.stripe_size;
    }


    void
    LvmLv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "lv-name", lv_name, rhs.lv_name);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "stripes", stripes, rhs.stripes);
	storage::log_diff(log, "stripe-size", stripe_size, rhs.stripe_size);
    }


    void
    LvmLv::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " lv-name:" << lv_name << " uuid:" << uuid;

	if (stripes != 0)
	    out << " stripes:" << stripes;
	if (stripe_size != 0)
	    out << " stripe-size:" << stripe_size;
    }


    ResizeInfo
    LvmLv::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = BlkDevice::Impl::detect_resize_info();

	// A logical volume must have at least one extent. Maximal size
	// calculated from free extents in volume group.

	const LvmVg* lvm_vg = get_lvm_vg();

	unsigned long long a = lvm_vg->get_extent_size();
	unsigned long long b = lvm_vg->get_impl().number_of_free_extents() + number_of_extents();

	resize_info.combine(ResizeInfo(true, a, b * a));

	return resize_info;
    }


    void
    LvmLv::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	BlkDevice::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_name() != lhs.get_name())
	{
	    Action::Base* action = new Action::Rename(get_sid());
	    actiongraph.add_vertex(action);
	    action->first = action->last = true;
	}
    }


    Text
    LvmLv::Impl::do_create_text(Tense tense) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by logical volume name (e.g. root),
			   // %2$s is replaced by size (e.g. 2GiB),
			   // %3$s is replaced by volume group name (e.g. system)
			   _("Create logical volume %1$s (%2$s) on volume group %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by logical volume name (e.g. root),
			   // %2$s is replaced by size (e.g. 2GiB),
			   // %3$s is replaced by volume group name (e.g. system)
			   _("Creating logical volume %1$s (%2$s) on volume group %3$s"));

	return sformat(text, lv_name.c_str(), get_size_string().c_str(),
		       lvm_vg->get_vg_name().c_str());
    }


    void
    LvmLv::Impl::do_create() const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	string cmd_line = LVCREATEBIN " --zero=y --wipesignatures=y --yes --name " + quote(lv_name) +
	    " --extents " + to_string(get_region().get_length());

	if (stripes > 1)
	{
	    cmd_line += " --stripes " + to_string(stripes);
	    if (stripe_size > 0)
		cmd_line += " --stripesize " + to_string(stripe_size / KiB);
	}

	cmd_line += " " + quote(lvm_vg->get_vg_name());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create LvmLv failed"));
    }


    Text
    LvmLv::Impl::do_rename_text(const Impl& lhs, Tense tense) const
    {
        return sformat(_("Rename %1$s to %2$s"), lhs.get_displayname().c_str(),
		       get_displayname().c_str());
    }


    void
    LvmLv::Impl::do_rename(const Impl& lhs) const
    {
    }


    Text
    LvmLv::Impl::do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				Tense tense) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	const LvmLv* lvm_lv_lhs = to_lvm_lv(lhs);
	const LvmLv* lvm_lv_rhs = to_lvm_lv(rhs);

	Text text;

	switch (resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by volume group name (e.g. system),
			      // %3$s is replaced by old size (e.g. 2GiB),
			      // %4$s is replaced by new size (e.g. 1GiB)
			      _("Shrink logical volume %1$s on volume group %2$s from %3$s to %4$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by volume group name (e.g. system),
			      // %3$s is replaced by old size (e.g. 2GiB),
			      // %4$s is replaced by new size (e.g. 1GiB)
			      _("Shrinking logical volume %1$s on volume group %2$s from %3$s to %4$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by volume group name (e.g. system),
			      // %3$s is replaced by old size (e.g. 1GiB),
			      // %4$s is replaced by new size (e.g. 2GiB)
			      _("Grow logical volume %1$s on volume group %2$s from %3$s to %4$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by volume group name (e.g. system),
			      // %3$s is replaced by old size (e.g. 1GiB),
			      // %4$s is replaced by new size (e.g. 2GiB)
			      _("Growing logical volume %1$s on volume group %2$s from %3$s to %4$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, lv_name.c_str(), lvm_vg->get_vg_name().c_str(),
		       lvm_lv_lhs->get_size_string().c_str(),
		       lvm_lv_rhs->get_size_string().c_str());
    }


    void
    LvmLv::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	const LvmLv* lvm_lv_rhs = to_lvm_lv(rhs);

	string cmd_line = LVRESIZEBIN;

	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --force";

	cmd_line += " " + quote(lvm_vg->get_vg_name() + "/" + lv_name) + " --extents " +
	    to_string(lvm_lv_rhs->get_region().get_length());

	cout << cmd_line << endl;

	wait_for_device();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize LvmLV failed"));
    }


    Text
    LvmLv::Impl::do_delete_text(Tense tense) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by logical volume name (e.g. root),
			   // %2$s is replaced by size (e.g. 2GiB),
			   // %3$s is replaced by volume group name (e.g. system)
			   _("Delete logical volume %1$s (%2$s) on volume group %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by logical volume name (e.g. root),
			   // %2$s is replaced by size (e.g. 2GiB),
			   // %3$s is replaced by volume group name (e.g. system)
			   _("Deleting logical volume %1$s (%2$s) on volume group %3$s"));

	return sformat(text, lv_name.c_str(), get_size_string().c_str(),
		       lvm_vg->get_vg_name().c_str());
    }


    void
    LvmLv::Impl::do_delete() const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	string cmd_line = LVREMOVEBIN " -f " + quote(lvm_vg->get_vg_name() + "/" + lv_name);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete LvmLv failed"));
    }


    namespace Action
    {

	Text
	Rename::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(get_device(actiongraph, LHS));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(get_device(actiongraph, RHS));
	    return rhs_lvm_lv->get_impl().do_rename_text(lhs_lvm_lv->get_impl(), tense);
	}

	void
	Rename::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(get_device(actiongraph, LHS));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(get_device(actiongraph, RHS));
	    return rhs_lvm_lv->get_impl().do_rename(lhs_lvm_lv->get_impl());
	}

    }


    string
    LvmLv::Impl::make_name(const string& vg_name, const string& lv_name)
    {
	return DEVDIR "/" + vg_name + "/" + lv_name;
    }


    string
    LvmLv::Impl::make_dm_table_name(const string& vg_name, const string& lv_name)
    {
	return boost::replace_all_copy(vg_name, "-", "--") + "-" +
	    boost::replace_all_copy(lv_name, "-", "--");
    }


    bool
    compare_by_lv_name(const LvmLv* lhs, const LvmLv* rhs)
    {
	return lhs->get_lv_name() < rhs->get_lv_name();
    }

}
