/*
 * Copyright (c) [2004-2010] Novell, Inc.
 * Copyright (c) 2023 SUSE LLC
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


#include "storage/SystemInfo/SystemInfoImpl.h"


namespace storage
{

    SystemInfo::Impl::Impl()
    {
	y2deb("constructed SystemInfo::Impl");
    }


    SystemInfo::Impl::~Impl()
    {
	y2deb("destructed SystemInfo::Impl");
    }


    const CmdUdevadmInfo&
    SystemInfo::Impl::getCmdUdevadmInfo(const string& file)
    {
	for (const auto& tmp : cmd_udevadm_infos.get_data())
	{
	    // does not have an object iff the constructor throwed
	    if (!tmp.second.has_object())
		continue;

	    const CmdUdevadmInfo& cmd_udevadm_info = tmp.second.get_object();
	    if (cmd_udevadm_info.is_alias_of(file))
		return cmd_udevadm_info;
	}

	return cmd_udevadm_infos.get(file);
    }

}
