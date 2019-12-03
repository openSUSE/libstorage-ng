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


#include "storage/CompoundAction/Formatter/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Utils/Format.h"


namespace storage
{

    CompoundAction::Formatter::LvmLv::LvmLv(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action, "LvmLv"),
	lv(to_lvm_lv(compound_action->get_target_device()))
    {}


    Text
    CompoundAction::Formatter::LvmLv::text() const
    {
	if (formatting() && is_swap(get_created_filesystem()))
	{
	    if (encrypting())
		return create_encrypted_with_swap_text();

	    else
		return create_with_swap_text();
	}

	else if (creating() && encrypting() && formatting() && mounting())
	    return create_encrypted_with_fs_and_mount_point_text();

	else if (creating() && encrypting() && formatting())
	    return create_encrypted_with_fs_text();

	else if (creating() && encrypting())
	    return create_encrypted_text();

	else if (creating() && formatting() && mounting())
	    return create_with_fs_and_mount_point_text();

	else if (creating() && formatting())
	    return create_with_fs_text();

	else if (creating())
	    return create_text();

	else if (encrypting() && formatting() && mounting())
	    return encrypted_with_fs_and_mount_point_text();

	else if (encrypting() && formatting())
	    return encrypted_with_fs_text();

	else if (encrypting())
	    return encrypted_text();

	else if (formatting() && mounting())
	    return fs_and_mount_point_text();

	else if (formatting())
	    return fs_text();

	else if (mounting())
	    return mount_point_text();

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s for swap");

	return sformat(text, get_lv_name(), get_size(), get_vg_name());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s for swap");

	return sformat(text, get_lv_name(), get_size(), get_vg_name());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(),
		       get_mount_point(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(), get_mount_point(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name());
    }


    Text
    CompoundAction::Formatter::LvmLv::encrypted_with_fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _("Encrypt LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(), get_mount_point(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::encrypted_with_fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _("Encrypt LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system)
	Text text = _("Encrypt LVM logical volume %1$s (%2$s) on volume group %3$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name());
    }


    Text
    CompoundAction::Formatter::LvmLv::fs_and_mount_point_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the mount point (e.g. /home),
	// %5$s is replaced with the file system name (e.g. ext4)
	Text text = _("Format LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(), get_mount_point(),
		       get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::fs_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the file system name (e.g. ext4)
	Text text = _("Format LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(), get_filesystem_type());
    }


    Text
    CompoundAction::Formatter::LvmLv::mount_point_text() const
    {
	string mount_point = get_created_mount_point()->get_path();

	// TRANSLATORS:
	// %1$s is replaced with the logical volume name (e.g. root),
	// %2$s is replaced with the size (e.g. 2.00 GiB),
	// %3$s is replaced with the volume group name (e.g. system),
	// %4$s is replaced with the mount point (e.g. /home)
	Text text = _("Mount LVM logical volume %1$s (%2$s) on volume group %3$s at %4$s");

	return sformat(text, get_lv_name(), get_size(), get_vg_name(), mount_point);
    }

}
