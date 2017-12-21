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


#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/SwapImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Swap>::classname = "Swap";


    Swap::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    ResizeInfo
    Swap::Impl::detect_resize_info() const
    {
	return ResizeInfo(true, min_size(), max_size());
    }


    ContentInfo
    Swap::Impl::detect_content_info() const
    {
	return ContentInfo();
    }


    SpaceInfo
    Swap::Impl::detect_space_info() const
    {
	ST_THROW(UnsupportedException("SpaceInfo unsupported for Swap"));
    }


    uint64_t
    Swap::Impl::used_features() const
    {
	return UF_SWAP | BlkFilesystem::Impl::used_features();
    }


    void
    Swap::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKSWAPBIN " -f " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create swap failed"));

	// TODO uuid is included in mkswap output

	probe_uuid();
    }


    void
    Swap::Impl::do_mount(CommitData& commit_data, const CommitOptions& commit_options,
        const MountPoint* mount_point) const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPONBIN " --fixpgsz " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("swapon failed"));
    }


    void
    Swap::Impl::do_umount(CommitData& commit_data, const MountPoint* mount_point) const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPOFFBIN " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("swapoff failed"));
    }


    void
    Swap::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	const BlkDevice* blk_device = get_blk_device();
	const BlkDevice* blk_device_rhs = to_swap(rhs)->get_impl().get_blk_device();

	string cmd_line = MKSWAPBIN;
	if (!get_label().empty())
	    cmd_line += " -L " + quote(get_label());
	if (!get_uuid().empty())
	    cmd_line += " -U " + quote(get_uuid());
	cmd_line += " " + quote(blk_device->get_name());
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " " + to_string(blk_device_rhs->get_size() / KiB);

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize swap failed"));
    }


    void
    Swap::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPLABELBIN " -L " + quote(get_label()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label swap failed"));
    }


    void
    Swap::Impl::do_set_uuid() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPLABELBIN " -U " + quote(get_uuid()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-uuid swap failed"));
    }

}
