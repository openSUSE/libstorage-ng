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


#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/EncryptionImpl.h"
#include "storage/Filesystems/SwapImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Redirect.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Swap>::classname = "Swap";


    Swap::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Swap::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Swap").translated;
    }


    ResizeInfo
    Swap::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(true, 0, min_size(), max_size());
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


    bool
    Swap::Impl::is_permanent() const
    {
	const BlkDevice* blk_device = get_blk_device();
	if (is_encryption(blk_device))
	{
	    const Encryption* encryption = to_encryption(blk_device);
	    if (encryption->get_impl().get_crypt_options().contains("swap"))
	    {
		return false;
	    }
	}

	return true;
    }


    void
    Swap::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKSWAP_BIN " -f " + quote(blk_device->get_name());

	if (!get_mkfs_options().empty())
	    cmd_line += " " + get_mkfs_options();

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	if (get_uuid().empty())
	{
	    // TODO uuid is included in mkswap output

	    probe_uuid();
	}
    }


    void
    Swap::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Swap* swap_rhs = to_swap(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_rhs = swap_rhs->get_impl().get_blk_device();

	string cmd_line = MKSWAP_BIN;
	if (!get_label().empty())
	    cmd_line += " -L " + quote(get_label());
	if (!get_uuid().empty())
	    cmd_line += " -U " + quote(get_uuid());
	cmd_line += " " + quote(action->blk_device->get_name());
	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_line += " " + to_string(blk_device_rhs->get_size() / KiB);

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Swap::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPLABEL_BIN " -L " + quote(get_label()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Swap::Impl::do_set_uuid() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPLABEL_BIN " -U " + quote(get_uuid()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Swap::Impl::immediate_activate(MountPoint* mount_point, bool force_rw) const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPON_BIN " --fixpgsz " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	if (mount_point->exists_in_system())
	    redirect_to_system(mount_point)->set_active(true);
    }


    void
    Swap::Impl::immediate_deactivate(MountPoint* mount_point) const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPOFF_BIN " " + quote(blk_device->get_name());

	try
	{
	    SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    SystemInfo::Impl system_info;

	    if (is_active_at_present(system_info, mount_point))
		ST_RETHROW(exception);

	    y2mil("ignoring swapoff failure since mount point seems already inactive");
	}

	if (mount_point->exists_in_system())
	    redirect_to_system(mount_point)->set_active(false);
    }

}
