/*
 * Copyright (c) 2017 SUSE LLC
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

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/ReiserfsImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Reiserfs>::classname = "Reiserfs";


    Reiserfs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    ResizeInfo
    Reiserfs::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = BlkFilesystem::Impl::detect_resize_info();

	resize_info.combine(ResizeInfo(true, 64 * MiB, 16 * TiB));

	return resize_info;
    }


    uint64_t
    Reiserfs::Impl::used_features() const
    {
	return UF_REISERFS | BlkFilesystem::Impl::used_features();
    }


    void
    Reiserfs::Impl::do_create() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFSREISERFSBIN " -f -f " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	blk_device->get_impl().wait_for_device();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create Reiserfs failed"));
    }


    void
    Reiserfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO handle mounted

	string cmd_line = TUNEREISERFSBIN " -l " + quote(get_label()) + " " +
	    quote(blk_device->get_name());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label Reiserfs failed"));
    }


    void
    Reiserfs::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	const BlkDevice* blk_device = get_blk_device();
	const BlkDevice* blk_device_rhs = to_reiserfs(rhs)->get_impl().get_blk_device();

	string cmd_line = REISERFSRESIZEBIN " -f";
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line = "echo y | " + cmd_line + " -s " +
		to_string(blk_device_rhs->get_size() / KiB) + "K";
	cmd_line += " " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	blk_device->get_impl().wait_for_device();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize Reiserfs failed"));
    }

}
