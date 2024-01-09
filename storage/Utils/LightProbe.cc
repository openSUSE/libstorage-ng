/*
 * Copyright (c) [2018-2024] SUSE LLC
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


#include "storage/Utils/LightProbe.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Prober.h"


namespace storage
{

    bool
    light_probe()
    {
	SystemInfo system_info;

	return light_probe(system_info);
    }


    bool
    light_probe(SystemInfo& system_info)
    {
	y2mil("light-probe begin");

	SysBlockEntries sys_block_entries = probe_sys_block_entries(system_info.get_impl());

	y2mil("light-probe end");

	return !sys_block_entries.disks.empty() || !sys_block_entries.dasds.empty();
    }

}
