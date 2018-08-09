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


#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/ExfatImpl.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Exfat>::classname = "Exfat";


    Exfat::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Exfat::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of a filesystem object in the storage devicegraph
	return _("exFAT").translated;
    }


    ContentInfo
    Exfat::Impl::detect_content_info_on_disk() const
    {
	EnsureMounted ensure_mounted(get_filesystem());

	ContentInfo content_info;

	content_info.is_windows = detect_is_windows(ensure_mounted.get_any_mount_point());

	return content_info;
    }


    uint64_t
    Exfat::Impl::used_features() const
    {
	return UF_EXFAT | BlkFilesystem::Impl::used_features();
    }


    void
    Exfat::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFS_EXFAT_BIN " " + get_mkfs_options() + " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	probe_uuid();
    }


    void
    Exfat::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = EXFAT_LABEL_BIN " " + quote(blk_device->get_name()) + " " +
	    quote(get_label());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
