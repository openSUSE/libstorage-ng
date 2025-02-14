/*
 * Copyright (c) 2025 SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/LvmDevicesFile.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    LvmDevicesFile::Status
    LvmDevicesFile::status()
    {
	SystemCmd::Options options({ LVMDEVICES_BIN }, SystemCmd::DoThrow);
	options.verify = [](int exit_code) { return exit_code == 0 || exit_code == 5; };

	SystemCmd cmd(options);

	if (cmd.retcode() == 0)
	    return Status::ENABLED;

	if (!cmd.stderr().empty())
	{
	    const string& line = cmd.stderr()[0];

	    if (boost::contains(line, "Devices file not enabled."))
		return Status::DISABLED;

	    if (boost::contains(line, "Devices file does not exist."))
		return Status::MISSING;
	}

	ST_THROW(Exception("unknown error of lvmdevices"));
    }


    void
    LvmDevicesFile::create(const Devicegraph* devicegraph)
    {
	// Note: Calling "vgimportdevices --all" does not add devices not belonging to a
	// volume group. The code here also adds those devices.

	vector<const LvmPv*> lvm_pvs = devicegraph->get_impl().get_devices_of_type<const LvmPv>();
	for (const LvmPv* lvm_pv : lvm_pvs)
	{
	    if (!lvm_pv->has_blk_device())
		continue;

	    const BlkDevice* blk_device = lvm_pv->get_blk_device();
	    SystemCmd cmd({ LVMDEVICES_BIN, "--adddev", blk_device->get_name() });
	}
    }

}
