/*
 * Copyright (c) [2017-2020] SUSE LLC
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
	return _("ReiserFS").translated;
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

	string cmd_line = MKFS_REISERFS_BIN " -f -f " + get_mkfs_options() + " " +
	    quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	// uuid is included in mkfs output

	probe_uuid();
    }


    void
    Reiserfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO handle mounted

	string cmd_line = TUNEREISERFS_BIN " -l " + quote(get_label()) + " " +
	    quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Reiserfs::Impl::do_set_tune_options() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNEREISERFS_BIN " " + get_tune_options() + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Reiserfs::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Reiserfs* reiserfs_rhs = to_reiserfs(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_rhs = reiserfs_rhs->get_impl().get_blk_device();

	string cmd_line = REISERFSRESIZE_BIN " -f";
	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_line = "echo y | " + cmd_line + " -s " +
		to_string(blk_device_rhs->get_size() / KiB) + "K";
	cmd_line += " " + quote(action->blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
