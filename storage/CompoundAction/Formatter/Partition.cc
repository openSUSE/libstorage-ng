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


#include "storage/CompoundAction/Formatter/Partition.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Filesystems/Swap.h"


namespace storage
{

    CompoundAction::Formatter::Partition::Partition(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action),
	partition(to_partition(compound_action->get_target_device()))
    {}


    Text
    CompoundAction::Formatter::Partition::text() const
    {
	if (has_delete<storage::Partition>())
	    return delete_text();

	else if (has_create<storage::LvmPv>())
	{
	    if (has_create<storage::Partition>() && has_create<storage::Encryption>())
		return create_encrypted_pv_text();

	    else if (has_create<storage::Partition>()) 
		return create_pv_text();

	    else if (has_create<storage::Encryption>())
		return encrypted_pv_text();

	    else
		return pv_text();
	}

	else if (has_create<storage::BlkFilesystem>() && is_swap(get_created_filesystem()))
	{
	    if (has_create<storage::Encryption>())
		return create_encrypted_with_swap_text();

	    else
		return create_with_swap_text();
	}

	else
	{
	    if (has_create<storage::Partition>() && has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
		return create_encrypted_with_fs_and_mount_point_text();

	    else if (has_create<storage::Partition>() && has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>())
		return create_encrypted_with_fs_text();

	    else if (has_create<storage::Partition>() && has_create<storage::Encryption>())
		return create_encrypted_text();

	    else if (has_create<storage::Partition>() && has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
		return create_with_fs_and_mount_point_text();

	    else if (has_create<storage::Partition>() && has_create<storage::BlkFilesystem>())
		return create_with_fs_text();

	    else if (has_create<storage::Partition>())
		return create_text();

	    else if (has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
		return encrypted_with_fs_and_mount_point_text();

	    else if (has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>())
		return encrypted_with_fs_text();

	    else if (has_create<storage::Encryption>())
		return encrypted_text();

	    else if (has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
		return fs_and_mount_point_text();

	    else if (has_create<storage::BlkFilesystem>())
		return fs_text();

	    else if (has_create<storage::MountPoint>())
		return mount_point_text();

	    else
		return default_text();
	}
    }

    
    Text
    CompoundAction::Formatter::Partition::delete_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Delete partition %1$s (%2$s)");

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_encrypted_pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create encrypted partition %1$s (%2$s) as LVM physical volume");

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create partition %1$s (%2$s) as LVM physical volume");

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::encrypted_pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create LVM physical volume over encrypted %1$s (%2$s)");

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::pv_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create LVM volume device over %1$s (%2$s)");

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_encrypted_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create encrypted partition %1$s (%2$s) for swap");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create partition %1$s (%2$s) for swap");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_encrypted_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create encrypted partition %1$s (%2$s) for %3$s with %4$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_encrypted_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create encrypted partition %1$s (%2$s) with %3$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB)
	Text text = _("Create encrypted partition %1$s (%2$s)");

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create partition %1$s (%2$s) for %3$s with %4$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create partition %1$s (%2$s) with %3$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by partition id string (e.g. Linux LVM)
	Text text = _("Create partition %1$s (%2$s) as %3$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       id_to_string(partition->get_id()).c_str());
    }


    Text
    CompoundAction::Formatter::Partition::encrypted_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Encrypt partition %1$s (%2$s) for %3$s with %4$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::encrypted_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Encrypt partition %1$s (%2$s) with %3$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	Text text = _("Encrypt partition %1$s (%2$s)");

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Format partition %1$s (%2$s) for %3$s with %4$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Format partition %1$s (%2$s) with %3$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::Partition::mount_point_text() const
    {
	auto mount_point = get_created_mount_point();

	// TRANSLATORS:
	// %1$s is replaced by partition name (e.g. /dev/sda1),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by mount point (e.g. /home)
	Text text = _("Mount partition %1$s (%2$s) at %3$s");

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       mount_point->get_path().c_str());
    }

}

