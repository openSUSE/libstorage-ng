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


#include "storage/CompoundAction/Formater/Partition.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Filesystems/MountPoint.h"


namespace storage
{

    CompoundAction::Formater::Partition::Partition(const CompoundAction::Impl* compound_action)
    : CompoundAction::Formater(compound_action) 
    {
	this->partition = to_partition(compound_action->get_target_device());
    }


    CompoundAction::Formater::Partition::~Partition() {}


    const BlkFilesystem*
    CompoundAction::Formater::Partition::get_created_filesystem() const
    {
	auto action = get_create<storage::BlkFilesystem>();

	if (action)
	{
	    auto device = CompoundAction::Impl::device(compound_action->get_actiongraph(), action);
	    return to_blk_filesystem(device);
	}

	return nullptr;
    }
    
    
    Text
    CompoundAction::Formater::Partition::text() const
    {
	if (has_delete<storage::Partition>())
	    return delete_text();

	else if (has_create<LvmPv>())
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
    CompoundAction::Formater::Partition::delete_text() const
    {
	Text text = tenser(tense,
			   _("Delete partition %1$s (%2$s)"),
			   _("Deleting partition %1$s (%2$s)"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_encrypted_pv_text() const
    {
	Text text = tenser(tense,
			   _("Create encrypted partition %1$s (%2$s) as LVM physical device"),
			   _("Creating encrypted partition %1$s (%2$s) as LVM physical device"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_pv_text() const
    {
	Text text = tenser(tense,
			   _("Create partition %1$s (%2$s) as LVM physical device"),
			   _("Creating partition %1$s (%2$s) as LVM physical device"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formater::Partition::encrypted_pv_text() const
    {
	Text text = tenser(tense,
			   _("Create encrypted LVM physical device over %1$s (%2$s)"),
			   _("Creating encrypted LVM physical device over %1$s (%2$s)"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formater::Partition::pv_text() const
    {
	Text text = tenser(tense,
			   _("Create LVM physical device over %1$s (%2$s)"),
			   _("Creating LVM physical device over %1$s (%2$s)"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_encrypted_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Create encrypted partition %1$s (%2$s) for %3$s with %4$s"),
			   _("Creating encrypted partition %1$s (%2$s) for %3$s with %4$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_encrypted_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Create encrypted partition %1$s (%2$s) with %3$s"),
			   _("Creating encrypted partition %1$s (%2$s) with %3$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_encrypted_text() const
    {
	Text text = tenser(tense,
			   _("Create encrypted partition %1$s (%2$s)"),
			   _("Creating encrypted partition %1$s (%2$s)"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Create partition %1$s (%2$s) for %3$s with %4$s"),
			   _("Creating partition %1$s (%2$s) for %3$s with %4$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Create partition %1$s (%2$s) with %3$s"),
			   _("Creating partition %1$s (%2$s) with %3$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::create_text() const
    {
	Text text = tenser(tense,
			   _("Create partition %1$s (%2$s) as %3$s"),
			   _("Creating partition %1$s (%2$s) as %3$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       id_to_string(partition->get_id()).c_str());
    }


    Text
    CompoundAction::Formater::Partition::encrypted_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Encrypt partition %1$s (%2$s) for %3$s with %4$s"),
			   _("Encrypting partition %1$s (%2$s) for %3$s with %4$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::encrypted_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Encrypt partition %1$s (%2$s) with %3$s"),
			   _("Encrypting partition %1$s (%2$s) with %3$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::encrypted_text() const
    {
	Text text = tenser(tense,
			   _("Encrypt partition %1$s (%2$s)"),
			   _("Encrypting partition %1$s (%2$s)"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Formater::Partition::fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Format partition %1$s (%2$s) for %3$s with %4$s"),
			   _("Formating partition %1$s (%2$s) for %3$s with %4$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::fs_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Format partition %1$s (%2$s) with %3$s"),
			   _("Formating partition %1$s (%2$s) with %3$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formater::Partition::mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	Text text = tenser(tense,
			   _("Mount partition %1$s (%2$s) at %3$s"),
			   _("Mounting partition %1$s (%2$s) at %3$s"));

	return sformat(text, 
		       partition->get_name().c_str(), 
		       partition->get_size_string().c_str(),
		       filesystem->get_mount_point()->get_path().c_str());
    }

}

