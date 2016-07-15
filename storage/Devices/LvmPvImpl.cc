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


#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/FindBy.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<LvmPv>::classname = "LvmPv";


    LvmPv::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    void
    LvmPv::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    bool
    LvmPv::Impl::has_blk_device() const
    {
	return has_single_parent_of_type<const BlkDevice>();
    }


    BlkDevice*
    LvmPv::Impl::get_blk_device()
    {
	return get_single_parent_of_type<BlkDevice>();
    }


    const BlkDevice*
    LvmPv::Impl::get_blk_device() const
    {
	return get_single_parent_of_type<const BlkDevice>();
    }


    void
    LvmPv::Impl::probe_lvm_pvs(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const CmdPvs::Pv& pv : systeminfo.getCmdPvs().get_pvs())
	{
	    LvmPv* lvm_pv = LvmPv::create(probed);
	    lvm_pv->get_impl().set_uuid(pv.pv_uuid);

	    // TODO the pv may not be included in any vg
	    LvmVg* lvm_vg = LvmVg::Impl::find_by_uuid(probed, pv.vg_uuid);
	    Subdevice::create(probed, lvm_pv, lvm_vg);
	}
    }


    void
    LvmPv::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	const CmdPvs& cmd_pvs = systeminfo.getCmdPvs();
	const CmdPvs::Pv& pv = cmd_pvs.find_by_pv_uuid(uuid);

	const BlkDevice* blk_device = BlkDevice::Impl::find_by_name(probed, pv.pv_name, systeminfo);
	User::create(probed, blk_device, get_device());
    }


    LvmPv*
    LvmPv::Impl::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<LvmPv>(devicegraph, uuid);
    }


    const LvmPv*
    LvmPv::Impl::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const LvmPv>(devicegraph, uuid);
    }


    bool
    LvmPv::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    LvmPv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    LvmPv::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " uuid:" << uuid;
    }


    Text
    LvmPv::Impl::do_create_text(Tense tense) const
    {
	return sformat(_("Create physical volume on %1$s"), "todo");
    }

}
