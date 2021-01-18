/*
 * Copyright (c) 2021 SUSE LLC
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


#include "storage/CompoundAction/Formatter/Tmpfs.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Utils/Format.h"


namespace storage
{

    CompoundAction::Formatter::Tmpfs::Tmpfs(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action),
	  tmpfs(to_tmpfs(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::Tmpfs::text() const
    {
	if (has_create<storage::MountPoint>())
	    return mount_text();

	else if (has_delete<storage::MountPoint>())
	    return unmount_text();

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::Tmpfs::mount_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the mount point (e.g. /tmp)
	Text text = _("Mount tmpfs at %1$s");

	return sformat(text, tmpfs->get_mount_point()->get_path());
    }


    Text
    CompoundAction::Formatter::Tmpfs::unmount_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the mount point (e.g. /tmp)
	Text text = _("Unmount tmpfs at %1$s");

	return sformat(text, tmpfs->get_mount_point()->get_path());
    }

}
