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
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/NtfsImpl.h"
#include "storage/SystemInfo/CmdNtfsresize.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Redirect.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Ntfs>::classname = "Ntfs";


    Ntfs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Ntfs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("NTFS").translated;
    }


    ResizeInfo
    Ntfs::Impl::detect_resize_info_on_disk(const BlkDevice* blk_device) const
    {
	if (!get_devicegraph()->get_impl().is_system() && !get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	const BlkDevice* fs_blk_device = get_blk_device();

	wait_for_devices();

	// TODO filesystem must not be mounted

	try
	{
	    CmdNtfsresize cmd_ntfsresize(fs_blk_device->get_name());

	    // See ntfsresize(8) for += 100 MiB.
	    ResizeInfo resize_info(true, 0, cmd_ntfsresize.get_min_size() + 100 * MiB, max_size());

	    // The min-size must never be bigger than the blk device size.
	    resize_info.min_size = min(resize_info.min_size, fs_blk_device->get_size());

	    if (resize_info.min_size >= resize_info.max_size)
		resize_info.reasons |= RB_FILESYSTEM_FULL;

	    return resize_info;
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    return ResizeInfo(false, RB_FILESYSTEM_INCONSISTENT);
	}
    }


    ContentInfo
    Ntfs::Impl::detect_content_info_on_disk() const
    {
	EnsureMounted ensure_mounted(get_filesystem());

	ContentInfo content_info;

	content_info.is_windows = detect_is_windows(ensure_mounted.get_any_mount_point());

	return content_info;
    }


    void
    Ntfs::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFS_NTFS_BIN " --fast --with-uuid " + get_mkfs_options() + " " +
	    quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	probe_uuid();
    }


    void
    Ntfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO labels starting with a '-' do not work (see bsc #1187746).

	SystemCmd::Args cmd_args = { NTFSLABEL_BIN, blk_device->get_name(), get_label() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    void
    Ntfs::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Ntfs* ntfs_rhs = to_ntfs(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_rhs = ntfs_rhs->get_impl().get_blk_device();

	string cmd_line = "echo y | " NTFSRESIZE_BIN " --force";
	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --size " + to_string(blk_device_rhs->get_size());
	cmd_line += " " + quote(action->blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
