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


#include "storage/CompoundAction/Formatter/Bcache.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    CompoundAction::Formatter::Bcache::Bcache(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action, "Bcache"),
	  bcache(to_bcache(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::Bcache::text() const
    {
	Text text = bcache_text();
	Text cset_text = bcache_cset_text();

	if (!cset_text.empty())
	{
	    text += UntranslatedText("\n");
	    text += cset_text;
	}

	return text;
    }


    Text
    CompoundAction::Formatter::Bcache::blk_devices_text() const
    {
	return join(bcache->get_bcache_cset()->get_blk_devices(), JoinMode::COMMA, 20);
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
	// Text related to cset is not shown for flash-only bcache devices
	if(bcache->get_type() == BcacheType::FLASH_ONLY)
	    return Text();

	// A bcache might not have an associated caching set
	if(!bcache->has_bcache_cset())
	    return Text();

	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the a list of cache devices with size
	// (e.g. "/dev/sdb1 (64.00 GiB) and /dev/sdc1 (160.00 GiB)")
	Text text = _( "%1$s is cached by %2$s" );

	return sformat(text, get_bcache_name(), blk_devices_text());
    }


    Text
    CompoundAction::Formatter::Bcache::delete_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	Text text = _( "Delete bcache %1$s on %2$s (%3$s)" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s) for swap" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Bcache::create_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	Text text = _( "Create bcache %1$s on %2$s (%3$s) for swap" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       get_mount_point(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::create_encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	Text text = _( "Create encrypted bcache %1$s on %2$s (%3$s)" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Bcache::create_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(), get_mount_point(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::create_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Create bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB)
	Text text = _( "Create bcache %1$s on %2$s (%3$s)" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       get_mount_point(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Encrypt bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	Text text = _( "Encrypt bcache %1$s on %2$s (%3$s)" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size());
    }


    Text
    CompoundAction::Formatter::Bcache::fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _( "Format bcache %1$s on %2$s (%3$s) for %4$s with %5$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       get_mount_point(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the file system type (e.g. ext4)
	Text text = _( "Format bcache %1$s on %2$s (%3$s) with %4$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::Bcache::mount_point_text() const
    {
	string mount_point = get_created_mount_point()->get_path();

	// TRANSLATORS:
	// %1$s is replaced with the bcache name (e.g. /dev/bcache0),
	// %2$s is replaced with the device name (e.g. /dev/sda1),
	// %3$s is replaced with the size (e.g. 2.00 GiB),
	// %4$s is replaced with the mount point (e.g. /home)
	Text text = _( "Mount bcache %1$s on %2$s (%3$s) at %4$s" );

	return sformat(text, get_bcache_name(), get_device_name(), get_size(),
		       mount_point);
    }


    const BlkDevice*
    CompoundAction::Formatter::Bcache::get_blk_device() const
    {
	if (bcache->get_type() == BcacheType::BACKED)
	{
	    return bcache->get_backing_device();
	}
	else
	{
	    vector<const BlkDevice*> caching_devices = bcache->get_bcache_cset()->get_blk_devices();

	    if(caching_devices.empty())
		ST_THROW(Exception("Flash-only bcache without caching device"));

	    return caching_devices.front();
	}
    }

}
