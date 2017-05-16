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


#ifndef STORAGE_BTRFS_SUBVOLUME_FORMATER_H
#define STORAGE_BTRFS_SUBVOLUME_FORMATER_H


#include "storage/CompoundAction/CompoundActionFormater.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Filesystems/BtrfsSubvolume.h"


namespace storage
{

    class BtrfsSubvolumeFormater : public CompoundActionFormater
    {

    public:

	BtrfsSubvolumeFormater(const CompoundAction::Impl* compound_action);
	~BtrfsSubvolumeFormater();

    private:

	const BlkDevice* get_blk_device() const;

	Text text() const;

	Text delete_text() const;

	Text create_with_no_copy_text() const;
	Text create_text() const;

    private:

	const BtrfsSubvolume* subvolume;

    };

}

#endif

