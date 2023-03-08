/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_ACTION_RENAME_IN_IMPL_H
#define STORAGE_ACTION_RENAME_IN_IMPL_H


#include "storage/Actions/ModifyImpl.h"
#include "storage/Devices/BlkDevice.h"


namespace storage
{

    namespace Action
    {

	/**
	 * Abstract class for renaming a blk device in some file, e.g. /etc/fstab or
	 * /etc/crypttab.
	 */
	class RenameIn : public Modify
	{
	public:

	    RenameIn(sid_t sid, const BlkDevice* blk_device) : Modify(sid), blk_device(blk_device) {}

	    const BlkDevice* get_renamed_blk_device(const Actiongraph::Impl& actiongraph,
						    Side side) const;

	private:

	    /**
	     * The blk device being renamed. Esp. important for btrfs
	     * which can have several blk devices.
	     */
	    const BlkDevice* blk_device;

	};

    }


    static_assert(std::is_abstract<Action::RenameIn>(), "RenameIn ought to be abstract.");

}

#endif
