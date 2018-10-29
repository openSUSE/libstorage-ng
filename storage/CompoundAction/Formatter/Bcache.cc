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


#include "storage/CompoundAction/Formatter/Bcache.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Filesystems/Swap.h"


namespace storage
{

    CompoundAction::Formatter::Bcache::Bcache(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action, "Bcache"),
	bcache(to_bcache(compound_action->get_target_device()))
    {}


    Text
    CompoundAction::Formatter::Bcache::text() const
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
    CompoundAction::Formatter::Bcache::format_as_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Format bcache %1$s (%2$s) as swap");

	return sformat(text, get_device_name().c_str(), get_size().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::format_as_encrypted_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Format bcache %1$s (%2$s) as encryped swap");

	return sformat(text, get_device_name().c_str(), get_size().c_str());
    }

    Text
    CompoundAction::Formatter::Bcache::encrypted_pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create LVM physical volume over encrypted %1$s (%2$s)");

	return sformat(text, get_device_name().c_str(), get_size().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create LVM physical volume over %1$s (%2$s)");

	return sformat(text, get_device_name().c_str(), get_size().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home),
	// %4$s is replaced by file system name (e.g. ext4)
	Text text = _("Encrypt bcache %1$s (%2$s) for %3$s with %4$s");

	return sformat(text,
		       get_device_name().c_str(),
		       get_size().c_str(),
		       get_mount_point().c_str(),
		       get_filesystem_type().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by file system name (e.g. ext4)
	Text text = _("Encrypt bcache %1$s (%2$s) with %3$s");

	return sformat(text,
		       get_device_name().c_str(),
		       get_size().c_str(),
		       get_filesystem_type().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB),
	Text text = _("Encrypt bcache %1$s (%2$s)");

	return sformat(text, get_device_name().c_str(), get_size().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home),
	// %4$s is replaced by file system name (e.g. ext4)
	Text text = _("Format bcache %1$s (%2$s) for %3$s with %4$s");

	return sformat(text,
		       get_device_name().c_str(),
		       get_size().c_str(),
		       get_mount_point().c_str(),
		       get_filesystem_type().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by file system name (e.g. ext4)
	Text text = _("Format bcache %1$s (%2$s) with %3$s");

	return sformat(text,
		       get_device_name().c_str(),
		       get_size().c_str(),
		       get_filesystem_type().c_str());
    }


    Text
    CompoundAction::Formatter::Bcache::mount_point_text() const
    {
	string mount_point = get_created_mount_point()->get_path();

	// TRANSLATORS:
	// %1$s is replaced by the bcache device name (e.g. /dev/bcache0),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home)
	Text text = _("Mount bcache %1$s (%2$s) at %3$s");

	return sformat(text,
		       get_device_name().c_str(),
		       get_size().c_str(),
                       mount_point.c_str());
    }

}
