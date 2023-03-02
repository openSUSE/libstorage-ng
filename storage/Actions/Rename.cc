/*
 * Copyright (c) [2016-2021] SUSE LLC
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


namespace storage
{

    namespace Action
    {

	Text
	Rename::text(const CommitData& commit_data) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(get_device(commit_data.actiongraph, LHS));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(get_device(commit_data.actiongraph, RHS));
	    return rhs_lvm_lv->get_impl().do_rename_text(lhs_lvm_lv->get_impl(), commit_data.tense);
	}

	void
	Rename::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(get_device(commit_data.actiongraph, LHS));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(get_device(commit_data.actiongraph, RHS));
	    return rhs_lvm_lv->get_impl().do_rename(lhs_lvm_lv->get_impl());
	}


	uf_t
	Rename::used_features(const Actiongraph::Impl& actiongraph) const
	{
	    const LvmLv* lvm_lv = to_lvm_lv(get_device(actiongraph, RHS));
	    return lvm_lv->get_impl().do_rename_used_features();
	}

    }

}
