/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#include "storage/CompoundAction/Formatter/Btrfs.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Utils/Format.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    CompoundAction::Formatter::Btrfs::Btrfs(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action, "Btrfs"),
	  btrfs(to_btrfs(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::Btrfs::blk_devices_text() const
    {
	return join(btrfs->get_blk_devices(), JoinMode::COMMA, 10);
    }


    Text
    CompoundAction::Formatter::Btrfs::text() const
    {
	if ( deleting() )
	    return delete_text();

	else if ( creating() )
	{
	    if ( mounting() )
		return create_and_mount_text();

	    else
		return create_text();
	}

	else if ( mounting() )
	    return mount_text();

	else if ( has_delete<storage::MountPoint>() )
	    return unmount_text();

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::Btrfs::delete_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the names of the devices with sizes (e.g. /dev/sda1
	//   (10.00 GiB and /dev/sda2 (10.00 GiB))
        Text text = _("Delete file system btrfs on %1$s");

        return sformat(text, blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Btrfs::create_and_mount_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the names of the devices with sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sda2 (10.00 GiB)),
	// %2$s is replaced with the mount point (e.g. /home)
        Text text = _("Create file system btrfs on %1$s and mount at %2$s");

        return sformat(text, blk_devices_text(),
		       btrfs->get_mount_point()->get_path());
    }


    Text
    CompoundAction::Formatter::Btrfs::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the names of the devices with sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sda2 (10.00 GiB))
        Text text = _("Create file system btrfs on %1$s");

        return sformat(text, blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Btrfs::mount_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the names of the devices with sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sda2 (10.00 GiB)),
	// %2$s is replaced with the mount point (e.g. /home)
        Text text = _("Mount file system btrfs on %1$s at %2$s");

        return sformat(text, blk_devices_text(), btrfs->get_mount_point()->get_path());
    }


    Text
    CompoundAction::Formatter::Btrfs::unmount_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the names of the devices with sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sda2 (10.00 GiB)),
	// %2$s is replaced with the mount point (e.g. /home)
        Text text = _("Unmount file system btrfs on %1$s at %2$s");

        return sformat(text, blk_devices_text(), btrfs->get_mount_point()->get_path());
    }


    CompoundAction::Formatter::BtrfsQuota::BtrfsQuota(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action, "BtrfsQuota"),
	  btrfs(to_btrfs(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::BtrfsQuota::blk_devices_text() const
    {
	return join(btrfs->get_blk_devices(), JoinMode::COMMA, 10);
    }


    Text
    CompoundAction::Formatter::BtrfsQuota::text() const
    {
	return default_text();
    }


    CompoundAction::Formatter::BtrfsQgroups::BtrfsQgroups(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action, "BtrfsQgroups"),
	  btrfs(to_btrfs(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::BtrfsQgroups::blk_devices_text() const
    {
	return join(btrfs->get_blk_devices(), JoinMode::COMMA, 10);
    }


    Text
    CompoundAction::Formatter::BtrfsQgroups::text() const
    {
	if (_compound_action->get_commit_actions().size() == 1)
	    return default_text();

	// TRANSLATORS:
	// %1$s is replaced with the names of the devices with sizes (e.g. /dev/sda1
	//   (10.00 GiB) and /dev/sda2 (10.00 GiB))
	Text text = _("Modify btrfs qgroups on %1$s");
	return sformat(text, blk_devices_text());
    }

}
