/*
 * Copyright (c) [2017-2021] SUSE LLC
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


#include "storage/Filesystems/UdfImpl.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/Math.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Udf>::classname = "Udf";


    Udf::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    unsigned long long
    Udf::Impl::max_size() const
    {
	const BlkDevice* blk_device = get_blk_device();

	unsigned int block_size = blk_device->get_region().get_block_size();

	if (block_size < 512 * B || block_size > 4 * KiB || !is_power_of_two(block_size))
	    ST_THROW(InvalidBlockSize(block_size));

	// max_size is e.g. 2 TiB for 512 B block size and 16 TiB for
	// 4 KiB block size.

	return 4 * GiB * block_size;
    }


    string
    Udf::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("UDF").translated;
    }


    void
    Udf::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = MKFS_UDF_BIN " " + get_mkfs_options() + " --label=" + quote(get_label());

	if (!get_uuid().empty())
	    cmd_line += " --uuid=" + quote(get_uuid());

	cmd_line += " " + quote(blk_device->get_name());

	wait_for_devices();

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	if (get_uuid().empty())
	{
	    probe_uuid();
	}
    }

}
