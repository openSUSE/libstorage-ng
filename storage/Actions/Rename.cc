/*
 * Copyright (c) [2016-2023] SUSE LLC
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


#include "storage/Actions/Rename.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devices/LvmVgImpl.h"


namespace storage
{

    namespace Action
    {

	Text
	Rename::text(const CommitData& commit_data) const
	{
	    const Device* device_rhs = get_device(commit_data.actiongraph, RHS);

	    if (is_lvm_lv(device_rhs))
	    {
		const LvmLv* rhs_lvm_lv = to_lvm_lv(device_rhs);
		return rhs_lvm_lv->get_impl().do_rename_text(commit_data, this);
	    }

	    if (is_lvm_vg(device_rhs))
	    {
		const LvmVg* rhs_lvm_vg = to_lvm_vg(device_rhs);
		return rhs_lvm_vg->get_impl().do_rename_text(commit_data, this);
	    }

	    ST_THROW(LogicException("Rename called for unsupported object"));
	}


	void
	Rename::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Device* device_rhs = get_device(commit_data.actiongraph, RHS);

	    if (is_lvm_lv(device_rhs))
	    {
		const LvmLv* rhs_lvm_lv = to_lvm_lv(device_rhs);
		return rhs_lvm_lv->get_impl().do_rename(commit_data, this);
	    }

	    if (is_lvm_vg(device_rhs))
	    {
		const LvmVg* rhs_lvm_vg = to_lvm_vg(device_rhs);
		return rhs_lvm_vg->get_impl().do_rename(commit_data, this);
	    }

	    ST_THROW(LogicException("Rename called for unsupported object"));
	}


	uf_t
	Rename::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const Device* device = get_device(actiongraph, RHS);

	    if (is_lvm_lv(device))
	    {
		const LvmLv* lvm_lv = to_lvm_lv(device);
		return lvm_lv->get_impl().do_rename_used_features();
	    }

	    if (is_lvm_vg(device))
	    {
		const LvmVg* lvm_vg = to_lvm_vg(device);
		return lvm_vg->get_impl().do_rename_used_features();
	    }

	    ST_THROW(LogicException("Rename called for unsupported object"));
	}

    }

}
