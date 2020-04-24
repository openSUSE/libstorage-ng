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


#ifndef STORAGE_FORMATTER_NFS_H
#define STORAGE_FORMATTER_NFS_H


#include "storage/CompoundAction/Formatter.h"
#include "storage/Filesystems/Nfs.h"


namespace storage
{

    class CompoundAction::Formatter::Nfs : public CompoundAction::Formatter
    {

    public:

	Nfs(const CompoundAction::Impl* compound_action);

    private:

	virtual Text text() const override;

	Text mount_text() const;
	Text unmount_text() const;

    private:

	const storage::Nfs* nfs;

    };

}

#endif
