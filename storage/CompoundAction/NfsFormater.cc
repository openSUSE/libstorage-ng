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


#include "storage/CompoundAction/NfsFormater.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Filesystems/FilesystemImpl.h"


namespace storage
{

    NfsFormater::NfsFormater(const CompoundAction::Impl* compound_action)
    : CompoundActionFormater(compound_action) 
    {
	this->nfs= to_nfs(compound_action->get_target_device());
    }


    NfsFormater::~NfsFormater() {}

    
    Text
    NfsFormater::text() const
    {
	if (has_create<MountPoint>())
	    return mount_text();

	else if (has_delete<MountPoint>())
	    return unmount_text();

	else
	    return default_text();
    }

    
    Text
    NfsFormater::mount_text() const
    {
        Text text = tenser(tense,
                           _("Mount NFS %1$s on %2$s"),
                           _("Mounting NFS %1$s on %2$s"));

        return sformat(text,
		       nfs->get_displayname().c_str(),
		       nfs->get_mount_point()->get_path().c_str());
    }


    Text
    NfsFormater::unmount_text() const
    {
        Text text = tenser(tense,
                           _("Unmount NFS %1$s at %2$s"),
                           _("Unmounting NFS %1$s at %2$s"));

        return sformat(text,
		       nfs->get_displayname().c_str(),
		       nfs->get_mount_point()->get_path().c_str());
    }

}

