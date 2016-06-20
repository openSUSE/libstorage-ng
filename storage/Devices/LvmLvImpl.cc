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


#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


using namespace std;


namespace storage
{

    const char* DeviceTraits<LvmLv>::classname = "LvmLv";


    LvmLv::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), lv_name(), uuid()
    {
	if (!getChildValue(node, "lv-name", lv_name))
	    ST_THROW(Exception("no lv-name"));

	getChildValue(node, "uuid", uuid);
    }


    void
    LvmLv::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "lv-name", lv_name);
	setChildValue(node, "uuid", uuid);
    }


    void
    LvmLv::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_1(probed, systeminfo);

	const CmdLvs& cmd_lvs = systeminfo.getCmdLvs();
	const CmdLvs::Lv& lv = cmd_lvs.find_by_lv_uuid(uuid);

	const LvmVg* lvm_vg = get_lvm_vg();

	unsigned long long extent_size = lvm_vg->get_region().get_block_size();
	set_region(Region(0, lv.size / extent_size, extent_size));
    }


    void
    LvmLv::Impl::set_lv_name(const string& lv_name)
    {
	Impl::lv_name = lv_name;

	// TODO call set_name()
    }


    const LvmVg*
    LvmLv::Impl::get_lvm_vg() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_lvm_vg(get_devicegraph()->get_impl()[vertex]);
    }


    bool
    LvmLv::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return lv_name == rhs.lv_name && uuid == rhs.uuid;
    }


    void
    LvmLv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "lv-name", lv_name, rhs.lv_name);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    LvmLv::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " lv-name:" << lv_name << " uuid:" << uuid;
    }


    void
    LvmLv::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
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

	return sformat(_("Create logical volume %1$s (%2$s) on volume group %3$s"),
		       get_displayname().c_str(), get_size_string().c_str(),
		       lvm_vg->get_displayname().c_str());
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


    namespace Action
    {

	Text
	Rename::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(get_device_lhs(actiongraph));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(get_device_rhs(actiongraph));
	    return rhs_lvm_lv->get_impl().do_rename_text(lhs_lvm_lv->get_impl(), tense);
	}

	void
	Rename::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(get_device_lhs(actiongraph));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(get_device_rhs(actiongraph));
	    return rhs_lvm_lv->get_impl().do_rename(lhs_lvm_lv->get_impl());
	}

    }


    string
    LvmLv::Impl::make_name(const string& vg_name, const string& lv_name)
    {
	return DEVDIR "/" + vg_name + "/" + lv_name;
    }


    bool
    compare_by_lv_name(const LvmLv* lhs, const LvmLv* rhs)
    {
	return lhs->get_lv_name() < rhs->get_lv_name();
    }

}
