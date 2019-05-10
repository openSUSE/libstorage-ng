/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Filesystems/ExtImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
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

	    Blkid::const_iterator it1 = blkid.find_by_name(blk_device->get_name(), prober.get_system_info());
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


    void
    Ext::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFSEXT2BIN " -t " + toString(get_type()) + " -v -F " +
	    get_mkfs_options() + " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	// TODO uuid is included in mkfs output

	probe_uuid();
    }


    void
    Ext::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNE2FSBIN " -L " + quote(get_label()) + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Ext::Impl::do_set_tune_options() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = TUNE2FSBIN " " + get_tune_options() + " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Ext::Impl::do_resize(ResizeMode resize_mode, const Device* rhs, const BlkDevice* blk_device) const
    {
	const BlkDevice* blk_device_rhs = to_ext(rhs)->get_impl().get_blk_device();

	string cmd_line = EXT2RESIZEBIN " -f " + quote(blk_device->get_name());
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " " + to_string(blk_device_rhs->get_size() / KiB) + "K";

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
