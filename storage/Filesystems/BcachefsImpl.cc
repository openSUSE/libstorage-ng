/*
 * Copyright (c) [2024-2026] SUSE LLC
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
#include "storage/Filesystems/BcachefsImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Bcachefs>::classname = "Bcachefs";


    Bcachefs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Bcachefs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Bcachefs").translated;
    }


    void
    Bcachefs::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	if (get_mkfs_options().empty())
	{
	    SystemCmd::Args cmd_args = { MKFS_BCACHEFS_BIN };

	    if (!get_label().empty())
		cmd_args << "--fs_label=" + get_label();

	    if (!get_uuid().empty())
		cmd_args << "--uuid=" + get_uuid();

	    cmd_args << get_mkfs_options_v2() << blk_device->get_name();

	    wait_for_devices();

	    SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
	}
	else
	{
	    string cmd_line = MKFS_BCACHEFS_BIN;

	    if (!get_label().empty())
		cmd_line += " --fs_label=" + quote(get_label());

	    if (!get_uuid().empty())
		cmd_line += " --uuid=" + quote(get_uuid());

	    cmd_line += " " + get_mkfs_options() + " " + quote(blk_device->get_name());

	    wait_for_devices();

	    SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
	}

	if (get_uuid().empty())
	{
	    // uuid is included in mkfs output

	    probe_uuid();
	}
    }

}
