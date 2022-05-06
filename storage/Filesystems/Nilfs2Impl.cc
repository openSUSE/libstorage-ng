/*
 * Copyright (c) 2022 SUSE LLC
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
#include "storage/Filesystems/Nilfs2Impl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Nilfs2>::classname = "Nilfs2";


    Nilfs2::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Nilfs2::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("NILFS2").translated;
    }


    void
    Nilfs2::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFS_NILFS2_BIN " -v " + get_mkfs_options() + " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	if (get_uuid().empty())
	{
	    probe_uuid();
	}
    }


    void
    Nilfs2::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = NILFS_TUNE_BIN " -L " + quote(get_label()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Nilfs2::Impl::do_set_uuid() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = NILFS_TUNE_BIN " -U " + quote(get_uuid()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Nilfs2::Impl::do_set_tune_options() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = NILFS_TUNE_BIN " " + get_tune_options() + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Nilfs2::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Nilfs2* nilfs2_rhs = to_nilfs2(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_rhs = nilfs2_rhs->get_impl().get_blk_device();

	EnsureMounted ensure_mounted(get_filesystem(), false);

	string cmd_line = NILFS_RESIZE_BIN " --yes " + quote(action->blk_device->get_name());
	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_line += " " + to_string(blk_device_rhs->get_size() / KiB) + "K";

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
