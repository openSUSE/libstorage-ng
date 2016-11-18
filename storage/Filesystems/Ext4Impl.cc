/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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

#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/Ext4Impl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Ext4>::classname = "Ext4";


    Ext4::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    ResizeInfo
    Ext4::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = Filesystem::Impl::detect_resize_info();

	resize_info.combine(ResizeInfo(true, 32 * MiB, 16 * TiB));

	return resize_info;
    }


    uint64_t
    Ext4::Impl::used_features() const
    {
	return UF_EXT4 | Filesystem::Impl::used_features();
    }


    void
    Ext4::Impl::do_create() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFSEXT2BIN " -t ext4 -v -F " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	blk_device->get_impl().wait_for_device();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create ext4 failed"));
    }


    void
    Ext4::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNE2FSBIN " -L " + quote(get_label()) + " " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label ext4 failed"));
    }


    void
    Ext4::Impl::do_resize(ResizeMode resize_mode) const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = EXT2RESIZEBIN " -f " + quote(blk_device->get_name());
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " " + to_string(blk_device->get_size() / KiB) + "K";
	cout << cmd_line << endl;

	blk_device->get_impl().wait_for_device();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize ext4 failed"));
    }

}
