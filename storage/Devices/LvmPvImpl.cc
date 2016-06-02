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
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"


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


    const BlkDevice*
    LvmPv::Impl::get_blk_device() const
    {
	return get_single_parent_of_type<const BlkDevice>();
    }


    void
    LvmPv::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	const CmdPvs& cmd_pvs = systeminfo.getCmdPvs();
	const CmdPvs::Pv& pv = cmd_pvs.find_by_pv_uuid(uuid);

	const BlkDevice* blk_device = BlkDevice::find_by_name(probed, pv.pv_name);
	User::create(probed, blk_device, get_device());
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
    }


    Text
    LvmPv::Impl::do_create_text(Tense tense) const
    {
	return sformat(_("Create physical volume on %1$s"), "todo");
    }

}
