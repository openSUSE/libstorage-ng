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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/NtfsImpl.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Redirect.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Ntfs>::classname = "Ntfs";


    Ntfs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Ntfs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("NTFS").translated;
    }


    ResizeInfo
    Ntfs::Impl::detect_resize_info_on_disk() const
    {
	if (!get_devicegraph()->get_impl().is_system() && !get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	const BlkDevice* blk_device = get_blk_device();

	wait_for_devices();

	ResizeInfo resize_info(false);

	// TODO filesystem must not be mounted

	SystemCmd cmd(NTFSRESIZEBIN " --force --info " + quote(blk_device->get_name()));
	if (cmd.retcode() != 0)
	    return resize_info;

	string fstr = " might resize at ";
	string::size_type pos;
	string stdout = boost::join(cmd.stdout(), "\n");
	if ((pos = stdout.find(fstr)) == string::npos)
	    return resize_info;

	resize_info.resize_ok = true;

	y2mil("pos:" << pos);
	pos = stdout.find_first_not_of(" \t\n", pos + fstr.size());
	y2mil("pos:" << pos);
	string number = stdout.substr(pos, stdout.find_first_not_of("0123456789", pos));
	number >> resize_info.min_size;

	// see ntfsresize(8) for += 100 MiB
	resize_info.min_size = min(resize_info.min_size + 100 * MiB, blk_device->get_size());
	resize_info.max_size = 256 * TiB - 64 * KiB;

	resize_info.check();

	return resize_info;
    }


    ContentInfo
    Ntfs::Impl::detect_content_info_on_disk() const
    {
	EnsureMounted ensure_mounted(get_filesystem());

	ContentInfo content_info;

	content_info.is_windows = detect_is_windows(ensure_mounted.get_any_mount_point());

	return content_info;
    }


    uint64_t
    Ntfs::Impl::used_features() const
    {
	return UF_NTFS | BlkFilesystem::Impl::used_features();
    }


    void
    Ntfs::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFSNTFSBIN " --fast --with-uuid " + get_mkfs_options() + " " +
	    quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create ntfs failed"));

	probe_uuid();
    }


    void
    Ntfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = NTFSLABELBIN " " + quote(blk_device->get_name()) + " " +
	    quote(get_label());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label ntfs failed"));
    }


    void
    Ntfs::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	const BlkDevice* blk_device = get_blk_device();
	const BlkDevice* blk_device_rhs = to_ntfs(rhs)->get_impl().get_blk_device();

	string cmd_line = "echo y | " NTFSRESIZEBIN " --force";
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --size " + to_string(blk_device_rhs->get_size());
	cmd_line += " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize ntfs failed"));
    }

}
