/*
 * Copyright (c) [2017-2019] SUSE LLC
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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include "storage/CompoundAction/Formatter/BtrfsSubvolume.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Utils/Format.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    CompoundAction::Formatter::BtrfsSubvolume::BtrfsSubvolume(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action),
	subvolume(to_btrfs_subvolume(compound_action->get_target_device()))
    {}


    Text
    CompoundAction::Formatter::BtrfsSubvolume::blk_devices_text() const
    {
	return join(subvolume->get_btrfs()->get_blk_devices(), JoinMode::COMMA, 10);
    }


    Text
    CompoundAction::Formatter::BtrfsSubvolume::text() const
    {
	if (has_delete<storage::BtrfsSubvolume>())
	    return delete_text();

	else if (has_create<storage::BtrfsSubvolume>())
	{
	    if (has_action<Action::SetNocow>())
		return create_with_no_copy_text();

	    else
		return create_text();
	}

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::BtrfsSubvolume::delete_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the subvolume path (e.g. var/log),
	// %2$s is replaced with the list of block device name and sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sdb1 (10.00 GiB))
        Text text = _("Delete subvolume %1$s on %2$s");

        return sformat(text, subvolume->get_path(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::BtrfsSubvolume::create_with_no_copy_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the subvolume path (e.g. var/log),
	// %2$s is replaced with the list of block device name and sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sdb1 (10.00 GiB))
        Text text = _("Create subvolume %1$s on %2$s with option 'no copy on write'");

        return sformat(text, subvolume->get_path(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::BtrfsSubvolume::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the subvolume path (e.g. var/log),
	// %2$s is replaced with the list of block device name and sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sdb1 (10.00 GiB))
        Text text = _("Create subvolume %1$s on %2$s");

        return sformat(text, subvolume->get_path(), blk_devices_text());
    }

}
