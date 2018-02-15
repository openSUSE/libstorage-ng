/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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
#include "storage/Holders/FilesystemUser.h"
#include "storage/Filesystems/XfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/UsedFeatures.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Xfs>::classname = "Xfs";


    Xfs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Xfs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("XFS").translated;
    }


    void
    Xfs::Impl::probe_pass_2b(Prober& prober)
    {
	BlkFilesystem::Impl::probe_pass_2b(prober);

	if (!get_uuid().empty())
	{
	    const Blkid& blkid = prober.get_system_info().getBlkid();
	    Blkid::const_iterator it = blkid.find_by_journal_uuid(get_uuid());
	    if (it != blkid.end())
	    {
		BlkDevice* jbd = BlkDevice::Impl::find_by_any_name(prober.get_probed(), it->first,
								   prober.get_system_info());
		FilesystemUser* filesystem_user = FilesystemUser::create(prober.get_probed(), jbd,
									 get_non_impl());
		filesystem_user->set_journal(true);
	    }
	}
    }


    uint64_t
    Xfs::Impl::used_features() const
    {
	return UF_XFS | BlkFilesystem::Impl::used_features();
    }


    void
    Xfs::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFSXFSBIN " -q -f -m crc=1 " + get_mkfs_options() + " " +
	    quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create xfs failed"));

	probe_uuid();
    }


    void
    Xfs::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	if (resize_mode == ResizeMode::SHRINK)
	    ST_THROW(Exception("shrink Xfs not possible"));

	EnsureMounted ensure_mounted(get_filesystem(), false);

        string cmd_line = XFSGROWFSBIN " " + ensure_mounted.get_any_mount_point();

        SystemCmd cmd(cmd_line);
        if (cmd.retcode() != 0)
            ST_THROW(Exception("resize Xfs failed"));
    }


    void
    Xfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = XFSADMINBIN " -L " + quote(get_label().empty() ? "--" : get_label()) + " " +
	    quote(blk_device->get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label xfs failed"));
    }

}
