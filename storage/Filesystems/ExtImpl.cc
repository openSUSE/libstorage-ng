/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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
#include "storage/Utils/HumanString.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Filesystems/ExtImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/SystemInfo/CmdDumpe2fs.h"
#include "storage/SystemInfo/CmdResize2fs.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Ext>::classname = "Ext";


    Ext::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    void
    Ext::Impl::probe_pass_2b(Prober& prober)
    {
	BlkFilesystem::Impl::probe_pass_2b(prober);

	if (supports_external_journal())
	{
	    const BlkDevice* blk_device = get_blk_device();

	    const Blkid& blkid = prober.get_system_info().getBlkid();

	    Blkid::const_iterator it1 = blkid.find_by_any_name(blk_device->get_name(), prober.get_system_info());
	    if (it1 != blkid.end() && !it1->second.fs_journal_uuid.empty())
	    {
		Blkid::const_iterator it2 = blkid.find_by_journal_uuid(it1->second.fs_journal_uuid);
		if (it2 != blkid.end())
		{
		    BlkDevice* jbd = BlkDevice::Impl::find_by_any_name(prober.get_system(), it2->first,
								       prober.get_system_info());
		    FilesystemUser* filesystem_user = FilesystemUser::create(prober.get_system(), jbd,
									     get_non_impl());
		    filesystem_user->set_journal(true);
		}
	    }
	}
    }


    unsigned long long
    Ext::Impl::max_size(unsigned long block_size, bool feature_64bit) const
    {
	// The max size depends on the block size and the 64bit feature (available in ext4).
	// The values have been researched in https://trello.com/c/LQVROAgq/.

	if (feature_64bit)
	    return (unsigned long long)(block_size) * (1ULL << 45);
	else
	    return (unsigned long long)(block_size) * (1ULL << 32);
    }


    ResizeInfo
    Ext::Impl::detect_resize_info_on_disk(const BlkDevice* blk_device) const
    {
	if (!get_devicegraph()->get_impl().is_system() && !get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	const BlkDevice* fs_blk_device = get_blk_device();

	wait_for_devices();

	try
	{
	    CmdDumpe2fs cmd_dumpe2fs(fs_blk_device->get_name());
	    CmdResize2fs cmd_resize2fs(fs_blk_device->get_name());

	    ResizeInfo resize_info(true, 0);

	    resize_info.min_size = cmd_resize2fs.get_min_blocks() * cmd_dumpe2fs.get_block_size();

	    // The min-size must never be bigger than the blk device size.
	    resize_info.min_size = min(resize_info.min_size, fs_blk_device->get_size());

	    resize_info.max_size = max_size(cmd_dumpe2fs.get_block_size(),
					    cmd_dumpe2fs.has_feature_64bit());

	    resize_info.combine_block_size(cmd_dumpe2fs.get_block_size());

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


    void
    Ext::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFS_EXT2_BIN " -t " + toString(get_type()) + " -v -F " +
	    get_mkfs_options() + " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	if (get_uuid().empty())
	{
	    // TODO uuid is included in mkfs output

	    probe_uuid();
	}
    }


    void
    Ext::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNE2FS_BIN " -L " + quote(get_label()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Ext::Impl::do_set_uuid() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNE2FS_BIN " -U " + quote(get_uuid()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Ext::Impl::do_set_tune_options() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNE2FS_BIN " " + get_tune_options() + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Ext::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Ext* ext_rhs = to_ext(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_rhs = ext_rhs->get_impl().get_blk_device();

	string cmd_line = RESIZE2FS_BIN " -f " + quote(action->blk_device->get_name());
	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_line += " " + to_string(blk_device_rhs->get_size() / KiB) + "K";

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
