/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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
#include "storage/Filesystems/VfatImpl.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Vfat>::classname = "Vfat";


    Vfat::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Vfat::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("VFAT").translated;
    }


    ContentInfo
    Vfat::Impl::detect_content_info_on_disk() const
    {
	EnsureMounted ensure_mounted(get_filesystem());

	ContentInfo content_info;

	if (detect_is_efi(ensure_mounted.get_any_mount_point()))
	    content_info.is_efi = true;
	else
	    content_info.is_windows = detect_is_windows(ensure_mounted.get_any_mount_point());

	return content_info;
    }


    void
    Vfat::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFS_FAT_BIN " -v " + get_mkfs_options() + " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	// uuid is included in mkfs output

	probe_uuid();
    }


    void
    Vfat::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO labels starting with a '-' do not work with dosfstools 4.2, a preceding
	// '--' is needed. But that does not work with dosfstools 4.1.

	SystemCmd::Args cmd_args = { FATLABEL_BIN, blk_device->get_name(), get_label() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    void
    Vfat::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	// The code here needs fatresize as shipped by SUSE as part of parted. See
	// https://bugzilla.suse.com/show_bug.cgi?id=1072479 for the problems with another
	// fatresize.

	const Vfat* vfat_rhs = to_vfat(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_rhs = vfat_rhs->get_impl().get_blk_device();

	SystemCmd::Args cmd_args = { FATRESIZE_BIN, action->blk_device->get_name() };
	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_args << to_string(blk_device_rhs->get_size() / KiB);

	wait_for_devices();

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }

}
