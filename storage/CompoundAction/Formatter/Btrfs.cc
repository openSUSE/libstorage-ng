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


#include <boost/algorithm/string/join.hpp>

#include "storage/CompoundAction/Formatter/Btrfs.h"
#include "storage/Filesystems/MountPoint.h"


namespace storage
{

    CompoundAction::Formatter::Btrfs::Btrfs(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action), 
	btrfs(to_btrfs(compound_action->get_target_device()))
    {}

    
    string
    CompoundAction::Formatter::Btrfs::blk_devices_string_representation() const
    {
	vector<string> names;
	for (auto device : btrfs->get_blk_devices())
	    names.push_back(device->get_displayname());

	return boost::algorithm::join(names, ", ");
    }
    
    
    Text
    CompoundAction::Formatter::Btrfs::text() const
    {
	if (has_delete<storage::Btrfs>())
	    return delete_text();

	else if (has_create<storage::Btrfs>())
	{
	    if (has_create<storage::MountPoint>())
		return create_and_mount_text();

	    else
		return create_text();
	}

	else if (has_create<storage::MountPoint>())
	    return mount_text();

	else if (has_delete<storage::MountPoint>())
	    return unmount_text();

	else
	    return default_text();
    }

    
    Text
    CompoundAction::Formatter::Btrfs::delete_text() const
    {
        Text text = tenser(tense,
                           _("Delete filesystem %1$s on %2$s"),
                           _("Deleting filesystem %1$s on %2$s"));

        return sformat(text,
		       btrfs->get_displayname().c_str(),
		       blk_devices_string_representation().c_str());
    }


    Text
    CompoundAction::Formatter::Btrfs::create_and_mount_text() const
    {
        Text text = tenser(tense,
                           _("Create filesystem %1$s at %3$s and mount on %2$s"),
                           _("Creating filesystem %1$s at %3$s and mounting on %2$s"));

        return sformat(text,
		       btrfs->get_displayname().c_str(),
		       btrfs->get_mount_point()->get_path().c_str(),
		       blk_devices_string_representation().c_str());
    }


    Text
    CompoundAction::Formatter::Btrfs::create_text() const
    {
        Text text = tenser(tense,
                           _("Create filesystem %1$s at %2$s"),
                           _("Creating filesystem %1$s at %2$s"));

        return sformat(text,
		       btrfs->get_displayname().c_str(),
		       blk_devices_string_representation().c_str());
    }


    Text
    CompoundAction::Formatter::Btrfs::mount_text() const
    {
        Text text = tenser(tense,
                           _("Mount filesystem %1$s on %2$s at %3$s"),
                           _("Mounting filesystem %1$s on %2$s at %3$s"));

        return sformat(text,
		       btrfs->get_displayname().c_str(),
		       btrfs->get_mount_point()->get_path().c_str(),
		       blk_devices_string_representation().c_str());
    }


    Text
    CompoundAction::Formatter::Btrfs::unmount_text() const
    {
        Text text = tenser(tense,
                           _("Unmount filesystem %1$s on %2$s at %3$s"),
                           _("Unmounting filesystem %1$s on %2$s at %3$s"));

        return sformat(text,
		       btrfs->get_displayname().c_str(),
		       btrfs->get_mount_point()->get_path().c_str(),
		       blk_devices_string_representation().c_str());
    }

}

