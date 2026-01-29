/*
 * Copyright (c) [2018-2019] SUSE LLC
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


#include "storage/CompoundAction/Formatter/Md.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Utils/Format.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    CompoundAction::Formatter::Md::Md(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action, "Md"),
	  md(to_md(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::Md::text() const
    {
	if ( deleting() )
	    return delete_text();

	else if ( formatting() && is_swap( get_created_filesystem() ) )
	{
	    if ( encrypting() )
		return create_encrypted_with_swap_text();

	    else
		return create_with_swap_text();
	}

	else
	{
	    if ( creating() && encrypting() && formatting() && mounting() )
		return create_encrypted_with_fs_and_mount_point_text();

	    else if ( creating() && encrypting() && formatting() )
		return create_encrypted_with_fs_text();

	    else if ( creating() && encrypting() )
		return create_encrypted_text();

	    else if ( creating() && formatting() && mounting() )
		return create_with_fs_and_mount_point_text();

	    else if ( creating() && formatting() )
		return create_with_fs_text();

	    else if ( creating() )
		return create_text();

	    else if ( encrypting() && formatting() && mounting() )
		return encrypted_with_fs_and_mount_point_text();

	    else if ( encrypting() && formatting() )
		return encrypted_with_fs_text();

	    else if ( encrypting() )
		return encrypted_text();

	    else if ( formatting() && mounting() )
		return fs_and_mount_point_text();

	    else if ( formatting() )
		return fs_text();

	    else if ( mounting() )
		return mount_point_text();

	    else
		return default_text();
	}
    }


    Text
    CompoundAction::Formatter::Md::blk_devices_text() const
    {
        return join(md->get_blk_devices(), JoinMode::COMMA, 20);
    }


    Text
    CompoundAction::Formatter::Md::delete_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0)
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	Text text = _( "Delete %1$s %2$s (%3$s)" );

	return sformat(text, get_md_level(), get_md_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Md::create_encrypted_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0)
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	// %4$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create encrypted %1$s %2$s (%3$s) for swap\nfrom %4$s" );
	text += _( "\nare you serious?!" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::create_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0)
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	// %4$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create %1$s %2$s (%3$s) for swap\nfrom %4$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::create_encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md name (e.g. /dev/md0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	// %6$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create encrypted %1$s %2$s (%3$s) for %4$s with %5$s\nfrom %6$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_mount_point(),
		       get_filesystem_type(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::create_encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md name (e.g. /dev/md0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	// %5$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create encrypted %1$s %2$s (%3$s) with %4$s\nfrom %5$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_filesystem_type(),
		       blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::create_encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md name (e.g. /dev/md0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	// %4$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create encrypted %1$s %2$s (%3$s)\nfrom %4$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::create_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	// %6$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create %1$s %2$s (%3$s) for %4$s with %5$s\nfrom %6$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_mount_point(),
		       get_filesystem_type(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::create_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md name (e.g. /dev/md0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	// %5$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create %1$s %2$s (%3$s) with %4$s\nfrom %5$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_filesystem_type(),
		       blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with a list of the devices
	//   the RAID is built from and their sizes: e.g.
	//   /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB) and /dev/sdc (512.00 GiB)
	Text text = _( "Create %1$s %2$s (%3$s) from %4$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Md::encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt %1$s %2$s (%3$s) for %4$s with %5$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_mount_point(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Md::encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt %1$s %2$s (%3$s) with %4$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Md::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	Text text = _( "Encrypt %1$s %2$s (%3$s)" );

	return sformat(text, get_md_level(), get_md_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Md::fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Format %1$s %2$s (%3$s) for %4$s with %5$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_mount_point(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Md::fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system type (e.g. ext4)
	Text text = _( "Format %1$s %2$s (%3$s) with %4$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Md::mount_point_text() const
    {
	string mount_point = get_created_mount_point()->get_path();

	// TRANSLATORS:
	// %1$s is replaced with the md level (e.g. RAID1),
	// %2$s is replaced with the md name (e.g. /dev/md0),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home)
	Text text = _( "Mount %1$s %2$s (%3$s) at %4$s" );

	return sformat(text, get_md_level(), get_md_name(), get_size(), mount_point);
    }

}
