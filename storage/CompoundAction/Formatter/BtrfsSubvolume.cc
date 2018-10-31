/*
 * Copyright (c) 2017 SUSE LLC
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


namespace storage
{

    CompoundAction::Formatter::BtrfsSubvolume::BtrfsSubvolume(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action),
	subvolume(to_btrfs_subvolume(compound_action->get_target_device()))
    {}


    const BlkDevice*
    CompoundAction::Formatter::BtrfsSubvolume::get_blk_device() const
    {
	return subvolume->get_btrfs()->get_impl().get_blk_device();
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
	// %2$s is replaced with the block device name (e.g. /dev/sda1)
        Text text = _("Delete subvolume %1$s on %2$s");

        return sformat(text, subvolume->get_path().c_str(), get_blk_device()->get_name().c_str());
    }


    Text
    CompoundAction::Formatter::BtrfsSubvolume::create_with_no_copy_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the subvolume path (e.g. var/log),
	// %2$s is replaced with the block device name (e.g. /dev/sda1)
        Text text = _("Create subvolume %1$s on %2$s with option 'no copy on write'");

        return sformat(text, subvolume->get_path().c_str(), get_blk_device()->get_name().c_str());
    }


    Text
    CompoundAction::Formatter::BtrfsSubvolume::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the subvolume path (e.g. var/log),
	// %2$s is replaced with the block device name (e.g. /dev/sda1)
        Text text = _("Create subvolume %1$s on %2$s");

        return sformat(text, subvolume->get_path().c_str(), get_blk_device()->get_name().c_str());
    }

}
