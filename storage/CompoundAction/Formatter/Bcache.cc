/*
 * Copyright (c) 2018 SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/CompoundAction/Formatter/Bcache.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Filesystems/Swap.h"



namespace storage
{

    CompoundAction::Formatter::Bcache::Bcache( const CompoundAction::Impl* compound_action ):
	CompoundAction::Formatter( compound_action, "Bcache" ),
	bcache( to_bcache( compound_action->get_target_device() ) ),
	bcache_cset( bcache->get_bcache_cset() )
    {
        // NOP
    }


    Text
    CompoundAction::Formatter::Bcache::text() const
    {
	Text text = bcache_text();
	Text cset_text = bcache_cset_text();

	if ( ! cset_text.translated.empty() )
	{
	    text += Text( "\n", "\n" );
	    text += cset_text;
	}

	return text;
    }


    Text
    CompoundAction::Formatter::Bcache::bcache_text() const
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
    CompoundAction::Formatter::Bcache::bcache_cset_text() const
    {
	Text dev_list_text;

	for ( const BlkDevice * device: bcache_cset->get_blk_devices() )
	{
	    if ( ! dev_list_text.translated.empty() )
		dev_list_text += Text( ", ", ", " );

	    // TRANSLATORS:
	    // %1$s is replaced with the the device name (e.g. /dev/sdc1),
	    // %2$s is replaced with the the size (e.g. 60 GiB)
	    Text dev_text = _( "%1$s (%2$s)" );

	    dev_list_text += sformat( dev_text,
                                      device->get_name().c_str(),
                                      device->get_size_string().c_str() );
	}

	// TRANSLATORS:
	// %1$s is replaced with the the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the a list of cache devices with size
	// (e.g. "/dev/sdb1 (64 GiB), /dev/sdc1 (160 GiB)")
	Text text = _( "%1$s is cached by %2$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        dev_list_text.translated.c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::delete_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Delete bcache %1$s on %2$s (%3$s)" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s) for swap" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Create bcache %1$s on %2$s (%3$s) for swap" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_mount_point().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s)" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_mount_point().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB)
	Text text = _( "Create bcache %1$s on %2$s (%3$s)" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_mount_point().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	Text text = _( "Encrypt bcache %1$s on %2$s (%3$s)" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Format bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_mount_point().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the file system type (e.g. ext4)
	Text text = _( "Format bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        get_filesystem_type().c_str() );
    }


    Text
    CompoundAction::Formatter::Bcache::mount_point_text() const
    {
	string mount_point = get_created_mount_point()->get_path();

	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2 GiB),
	// %4$s is replaced with the mount point (e.g. /home)
	Text text = _( "Mount bcache %1$s on %2$s (%3$s) at %4$s" );

	return sformat( text,
                        get_bcache_name().c_str(),
                        get_device_name().c_str(),
                        get_size().c_str(),
                        mount_point.c_str() );
    }

}
