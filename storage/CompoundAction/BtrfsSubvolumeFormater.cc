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


#include "storage/CompoundAction/BtrfsSubvolumeFormater.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"


namespace storage
{

    BtrfsSubvolumeFormater::BtrfsSubvolumeFormater(const CompoundAction::Impl* compound_action)
    : CompoundActionFormater(compound_action) 
    {
	this->subvolume = to_btrfs_subvolume(compound_action->get_target_device());
    }


    BtrfsSubvolumeFormater::~BtrfsSubvolumeFormater() {}

    
    const BlkDevice*
    BtrfsSubvolumeFormater::get_blk_device() const
    {
	return subvolume->get_btrfs()->get_impl().get_blk_device(); 
    }

    
    Text
    BtrfsSubvolumeFormater::text() const
    {
	if (has_delete<BtrfsSubvolume>())
	    return delete_text();

	else if (has_create<BtrfsSubvolume>())
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
    BtrfsSubvolumeFormater::delete_text() const
    {
        Text text = tenser(tense,
                           _("Delete subvolume %1$s on %2$s"),
                           _("Deleting subvolume %1$s on %2$s"));

        return sformat(text, subvolume->get_path().c_str(), get_blk_device()->get_name().c_str());
    }


    Text
    BtrfsSubvolumeFormater::create_with_no_copy_text() const
    {
        Text text = tenser(tense,
                           _("Create subvolume subvolume %1$s on %2$s with option 'no copy on write'"),
                           _("Creating subvolume subvolume %1$s on %2$s with option 'no copy on write'"));

        return sformat(text, subvolume->get_path().c_str(), get_blk_device()->get_name().c_str());
    }


    Text
    BtrfsSubvolumeFormater::create_text() const
    {
        Text text = tenser(tense,
                           _("Create subvolume subvolume %1$s on %2$s"),
                           _("Creating subvolume subvolume %1$s on %2$s"));

        return sformat(text, subvolume->get_path().c_str(), get_blk_device()->get_name().c_str());
    }

}

