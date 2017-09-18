/*
 * Copyright (c) [2014-2015] Novell, Inc.
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

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Filesystem>::classname = "Filesystem";


    Filesystem::Impl::Impl(const xmlNode* node)
	: Mountable::Impl(node)
    {
	const xmlNode* space_info_node = getChildNode(node, "SpaceInfo");
	if (space_info_node)
	    space_info.set_value(SpaceInfo(space_info_node));
    }


    void
    Filesystem::Impl::save(xmlNode* node) const
    {
	Mountable::Impl::save(node);

	if (space_info.has_value())
	{
	    xmlNode* space_info_node = xmlNewChild(node, "SpaceInfo");
	    space_info.get_value().save(space_info_node);
	}
    }


    bool
    Filesystem::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Mountable::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    Filesystem::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Mountable::Impl::log_diff(log, rhs);
    }


    SpaceInfo
    Filesystem::Impl::detect_space_info() const
    {
	if (!space_info.has_value())
	{
	    space_info.set_value(detect_space_info_on_disk());
	}

	return space_info.get_value();
    }


    SpaceInfo
    Filesystem::Impl::detect_space_info_on_disk() const
    {
	EnsureMounted ensure_mounted(get_filesystem());

	SystemInfo system_info;
	const CmdDf& cmd_df = system_info.getCmdDf(ensure_mounted.get_any_mount_point());

	return cmd_df.get_space_info();
    }


    void
    Filesystem::Impl::set_space_info(const SpaceInfo& tmp)
    {
	space_info.set_value(tmp);
    }


    void
    Filesystem::Impl::print(std::ostream& out) const
    {
	Mountable::Impl::print(out);
    }

}
