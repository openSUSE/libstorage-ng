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


#include "storage/CompoundAction/Formatter/Nfs.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Utils/Format.h"


namespace storage
{

    CompoundAction::Formatter::Nfs::Nfs(const CompoundAction::Impl* compound_action)
	: CompoundAction::Formatter(compound_action),
	  nfs(to_nfs(compound_action->get_target_device()))
    {
    }


    Text
    CompoundAction::Formatter::Nfs::text() const
    {
	if (has_create<storage::MountPoint>())
	    return mount_text();

	else if (has_delete<storage::MountPoint>())
	    return unmount_text();

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::Nfs::mount_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the nfs name (e.g. 99.99.00.00:path),
	// %2$s is replaced with the mount point (e.g. /home)
        Text text = _("Mount NFS %1$s on %2$s");

        return sformat(text, nfs->get_displayname(), nfs->get_mount_point()->get_path());
    }


    Text
    CompoundAction::Formatter::Nfs::unmount_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced with the nfs name (e.g. 99.99.00.00:path),
	// %2$s is replaced with the mount point (e.g. /home)
        Text text = _("Unmount NFS %1$s at %2$s");

        return sformat(text, nfs->get_displayname(), nfs->get_mount_point()->get_path());
    }

}
