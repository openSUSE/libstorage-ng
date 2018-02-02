/*
 * Copyright (c) [2017-2018] SUSE LLC
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


    string
    Reiserfs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Reiserfs").translated;
    }


    uint64_t
    Reiserfs::Impl::used_features() const
    {
	return UF_REISERFS | BlkFilesystem::Impl::used_features();
    }


    void
    Reiserfs::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFSREISERFSBIN " -f -f " + get_mkfs_options() + " " +
	    quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create Reiserfs failed"));

	// uuid is included in mkfs output

	probe_uuid();
    }


    void
    Reiserfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO handle mounted

	string cmd_line = TUNEREISERFSBIN " -l " + quote(get_label()) + " " +
	    quote(blk_device->get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label Reiserfs failed"));
    }


    void
    Reiserfs::Impl::do_set_tune_options() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNEREISERFSBIN " " + get_tune_options() + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-tune-options Reiserfs failed"));
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

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize Reiserfs failed"));
    }

}
