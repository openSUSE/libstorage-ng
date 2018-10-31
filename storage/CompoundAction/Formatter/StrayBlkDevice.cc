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


#include "storage/CompoundAction/Formatter/StrayBlkDevice.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Filesystems/Swap.h"


namespace storage
{

    CompoundAction::Formatter::StrayBlkDevice::StrayBlkDevice( const CompoundAction::Impl* compound_action ):
	CompoundAction::Formatter( compound_action, "StrayBlkDevice" ),
	stray_blk_device( to_stray_blk_device( compound_action->get_target_device( ) ) )
    {
        // NOP
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::text() const
    {
	if ( has_create<storage::LvmPv>() )
	{
	    if ( encrypting() )
		return encrypted_pv_text();
	    else
		return pv_text();
	}
	else if ( formatting() && is_swap( get_created_filesystem() ) )
	{
	    if ( encrypting() )
                return format_as_encrypted_swap_text();
            else
                return format_as_swap_text();
	}
	else
	{
	    if ( encrypting() && formatting() && mounting() )
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
    CompoundAction::Formatter::StrayBlkDevice::format_as_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Format partition %1$s (%2$s) as swap" );

	return sformat( text, get_device_name().c_str(), get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::format_as_encrypted_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Format partition %1$s (%2$s) as encryped swap" );

	return sformat( text, get_device_name().c_str(), get_size().c_str() );
    }

    Text
    CompoundAction::Formatter::StrayBlkDevice::encrypted_pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Create LVM physical volume over encrypted %1$s (%2$s)" );

	return sformat( text, get_device_name().c_str(), get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Create LVM physical volume over %1$s (%2$s)" );

	return sformat( text, get_device_name().c_str(), get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB),
	// %3$s is replaced with the mount point (e.g. /home),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt partition %1$s (%2$s) for %3$s with %4$s" );

	return sformat( text,
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_mount_point().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB),
	// %3$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt partition %1$s (%2$s) with %3$s" );

	return sformat( text,
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB),
	Text text = _( "Encrypt partition %1$s (%2$s)" );

	return sformat( text, get_device_name().c_str(), get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB),
	// %3$s is replaced with the mount point (e.g. /home),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Format partition %1$s (%2$s) for %3$s with %4$s" );

	return sformat( text,
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_mount_point().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB),
	// %3$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Format partition %1$s (%2$s) with %3$s" );

	return sformat( text,
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::StrayBlkDevice::mount_point_text() const
    {
	string mount_point = get_created_mount_point()->get_path();

	// TRANSLATORS:
	// %1$s is replaced with the partition name (e.g. /dev/sda1),
	// %2$s is replaced with the size (e.g. 2 GiB),
	// %3$s is replaced with the mount point (e.g. /home)
	Text text = _( "Mount partition %1$s (%2$s) at %3$s" );

	return sformat( text,
                        get_device_name().c_str(),
                        get_size().c_str(),
                        mount_point.c_str() );
    }

}
