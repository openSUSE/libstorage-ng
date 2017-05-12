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


#include <boost/algorithm/string/join.hpp>

#include "storage/CompoundActionImpl.h"
#include "storage/ActiongraphImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"


namespace storage
{

    CompoundAction::Impl::Impl(const Actiongraph* actiongraph)
    : actiongraph(actiongraph), target_device(nullptr) 
    {}


    CompoundAction::Impl::~Impl() {}


    void CompoundAction::Impl::set_target_device(const Device* device)
    {
	this->target_device = device;
    }


    const Device* CompoundAction::Impl::get_target_device() const
    {
	return target_device;
    }

    void CompoundAction::Impl::set_commit_actions(vector<const Action::Base*> actions)
    {
	this->commit_actions = actions;
    }


    vector<const Action::Base*> CompoundAction::Impl::get_commit_actions() const
    {
	return commit_actions;
    }


    void CompoundAction::Impl::add_commit_action(const Action::Base* action)
    {
	commit_actions.push_back(action);
    }


    vector<string>
    CompoundAction::Impl::get_commit_actions_as_strings() const
    {
	const CommitData commit_data(actiongraph->get_impl(), Tense::SIMPLE_PRESENT);

	vector<string> ret;
	for (const Action::Base* action : commit_actions)
	    ret.push_back(action->text(commit_data).translated);

	return ret;
    }


    bool
    CompoundAction::Impl::is_delete() const
    {
	for(auto action : commit_actions)
	{
	    if (storage::is_delete(action))
		return true;
	}

	return false;
    }


    //TODO Generate text depending on the set of commit actions.
    string
    CompoundAction::Impl::to_string() const
    {
	if (is_partition(target_device))
	    return to_string(to_partition(target_device));

	else if (is_lvm_lv(target_device))
	    return to_string(to_lvm_lv(target_device));

	else
	    return boost::algorithm::join(get_commit_actions_as_strings(), " and ");	
    }


    string
    CompoundAction::Impl::to_string(const Partition* partition) const
    {
	if (is_delete())
	    return delete_partition_text(partition, Tense::SIMPLE_PRESENT).translated;
	else
	    return create_partition_text(partition, Tense::SIMPLE_PRESENT).translated;
    }


    Text
    CompoundAction::Impl::delete_partition_text(const Partition* partition, Tense tense) const
    {
	Text text = tenser(tense,
			   _("Delete partition %1$s (%2$s)"),
			   _("Deleting partition %1$s (%2$s)"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Impl::create_partition_text(const Partition* partition, Tense tense) const
    {
	if (has_encryption() && has_pv())
	    return create_encrypted_pv_partition_text(partition, Tense::SIMPLE_PRESENT);

	else if (has_pv())
	    return create_pv_partition_text(partition, Tense::SIMPLE_PRESENT);

	else if (has_encryption())
	    return create_encrypted_plane_partition_text(partition, Tense::SIMPLE_PRESENT);

	else
	    return create_plane_partition_text(partition, Tense::SIMPLE_PRESENT);
    }


    Text
    CompoundAction::Impl::create_encrypted_pv_partition_text(const Partition* partition, Tense tense) const
    {
	Text text = tenser(tense,
			   _("Create encrypted partition %1$s (%2$s) for PV"),
			   _("Creating encrypted partition %1$s (%2$s) for PV"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }

    
    Text
    CompoundAction::Impl::create_pv_partition_text(const Partition* partition, Tense tense) const
    {
	Text text = tenser(tense,
			   _("Create partition %1$s (%2$s) for PV"),
			   _("Creating partition %1$s (%2$s) for PV"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Impl::create_encrypted_plane_partition_text(const Partition* partition, Tense tense) const
    {
	Text text = tenser(tense,
			   _("Create encrypted partition %1$s (%2$s) with fs for path"),
			   _("Creating encrypted partition %1$s (%2$s) with fs for path"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    Text
    CompoundAction::Impl::create_plane_partition_text(const Partition* partition, Tense tense) const
    {
	Text text = tenser(tense,
			   _("Create partition %1$s (%2$s) with fs for path"),
			   _("Creating partition %1$s (%2$s) with fs for path"));

	return sformat(text, partition->get_name().c_str(), partition->get_size_string().c_str());
    }


    bool
    CompoundAction::Impl::has_encryption() const
    {
	return false;
    }


    bool
    CompoundAction::Impl::has_pv() const
    {
	return false;
    }


    string
    CompoundAction::Impl::to_string(const LvmLv* lv) const
    {
	return create_lvm_lv_text(lv, Tense::SIMPLE_PRESENT).translated;
    }


    Text
    CompoundAction::Impl::create_lvm_lv_text(const LvmLv* lv, Tense tense) const
    {
	auto vg = lv->get_lvm_vg();
	auto filesystem = lv->get_blk_filesystem();

	Text text = tenser(tense,
			   _("Create LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s"),
			   _("Creating LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s"));

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       vg->get_vg_name().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }

}

