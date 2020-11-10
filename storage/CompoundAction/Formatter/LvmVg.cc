/*
 * Copyright (c) [2017-2019] SUSE LLC
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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include <vector>

#include "storage/CompoundAction/Formatter/LvmVg.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Utils/Format.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    CompoundAction::Formatter::LvmVg::LvmVg(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action, "LvmVg"),
	  vg(to_lvm_vg(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::LvmVg::blk_devices_text() const
    {
	vector<const BlkDevice*> blk_devices;
	for (const LvmPv* pv : vg->get_lvm_pvs())
	    blk_devices.push_back(pv->get_blk_device());

	return join(blk_devices, JoinMode::COMMA, 20);
    }


    Text
    CompoundAction::Formatter::LvmVg::text() const
    {
	if ( creating() )
	{
	    if (vg->get_lvm_pvs().size() > 0)
		return create_with_pvs_text();

	    else
		return create_text();
	}

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::LvmVg::create_with_pvs_text() const
    {
	// TRANSLATORS: displayed before action,
	// %1$s is replaced with the volume group name (e.g. system),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the name of devices and their sizes (e.g. /dev/sda1
	//   (10.00 GiB and /dev/sda2 (10.00 GiB))
	Text text = _("Create volume group %1$s (%2$s) with %3$s");

	return sformat(text, vg->get_vg_name(), vg->get_size_string(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::LvmVg::create_text() const
    {
	// TRANSLATORS: displayed before action,
	// %1$s is replaced with the volume group name (e.g. system),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	Text text = _("Create volume group %1$s (%2$s)");

	return sformat(text, vg->get_vg_name(), vg->get_size_string());
    }

}
