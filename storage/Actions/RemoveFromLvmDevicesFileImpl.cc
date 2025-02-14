/*
 * Copyright (c) [2017-2025] SUSE LLC
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


#include "storage/Actions/RemoveFromLvmDevicesFileImpl.h"
#include "storage/Devices/LvmPvImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	RemoveFromLvmDevicesFile::text(const CommitData& commit_data) const
	{
	    const LvmPv* lvm_pv = to_lvm_pv(get_device(commit_data.actiongraph, LHS));
	    return lvm_pv->get_impl().do_remove_from_lvm_devices_file_text(commit_data.tense);
	}


	void
	RemoveFromLvmDevicesFile::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const LvmPv* lvm_pv = to_lvm_pv(get_device(commit_data.actiongraph, LHS));
	    lvm_pv->get_impl().do_remove_from_lvm_devices_file(commit_data);
	}

    }

}
