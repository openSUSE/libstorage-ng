/*
 * Copyright (c) 2018 SUSE LLC
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


#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/F2fsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<F2fs>::classname = "F2fs";


    F2fs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    F2fs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("F2FS").translated;
    }


    void
    F2fs::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFS_F2FS_BIN " " + get_mkfs_options();

	if (!get_label().empty())
	    cmd_line += " -l " + quote(get_label());

	cmd_line += " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	probe_uuid();
    }

}
