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


#include "storage/Actions/RenameInImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    namespace Action
    {

	const BlkDevice*
	RenameIn::get_renamed_blk_device(const Actiongraph::Impl& actiongraph, Side side) const
	{
	    return to_blk_device(actiongraph.get_devicegraph(side)->find_device(blk_device->get_sid()));
	}

    }

}
