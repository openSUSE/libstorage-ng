/*
 * Copyright (c) [2017-2022] SUSE LLC
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


#include <algorithm>

#include "storage/CompoundAction/Generator.h"
#include "storage/CompoundActionImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Filesystems/BtrfsQgroupImpl.h"
#include "storage/Holders/BtrfsQgroupRelationImpl.h"
#include "storage/Redirect.h"
#include "storage/Actions/SetQuotaImpl.h"


namespace storage
{

    CompoundAction::Generator::Generator(const Actiongraph* actiongraph)
	: actiongraph(actiongraph)
    {
    }


    vector<shared_ptr<CompoundAction>>
    CompoundAction::Generator::generate() const
    {
	vector<shared_ptr<CompoundAction>> compound_actions;

	for (const Action::Base* action : actiongraph->get_commit_actions())
	{
	    pair<const Device*, CompoundAction::Impl::Type> meta_device = get_meta_device(action);

	    shared_ptr<CompoundAction> compound_action = find_by_target_device(compound_actions, meta_device.first,
									       meta_device.second);

	    if (compound_action)
	    {
		compound_action->get_impl().add_commit_action(action);
	    }
	    else
	    {
		compound_action = make_shared<CompoundAction>(actiongraph);
		compound_action->get_impl().set_target_device(meta_device.first);
		compound_action->get_impl().set_type(meta_device.second);
		compound_action->get_impl().add_commit_action(action);
		compound_actions.push_back(compound_action);
	    }
	}

	return compound_actions;
    }


    pair<const Device*, CompoundAction::Impl::Type>
    CompoundAction::Generator::get_meta_device(const Action::Base* action) const
    {
	// TODO The code here is not nice, just like
	// CompoundAction::Impl::get_target_device et.al. is not nice.  Maybe it would be
	// better if the actions would return the device.

	if (action->affects_device())
	{
	    const Action::SetQuota* set_quota_action = dynamic_cast<const Action::SetQuota*>(action);
	    if (set_quota_action)
	    {
		const Btrfs* btrfs = to_btrfs(set_quota_action->get_device(actiongraph->get_impl(), RHS));
		return make_pair(btrfs, CompoundAction::Impl::Type::BTRFS_QUOTA);
	    }

	    const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	    if (create_action && is_btrfs_qgroup(create_action->get_device(actiongraph->get_impl())))
	    {
		const BtrfsQgroup* tmp = to_btrfs_qgroup(create_action->get_device(actiongraph->get_impl()));
		if (tmp->get_impl().has_btrfs_subvolume())
		{
		    const BtrfsSubvolume* btrfs_subvolume = tmp->get_impl().get_btrfs_subvolume();
		    return make_pair(btrfs_subvolume, CompoundAction::Impl::Type::NORMAL);
		}
		else
		{
		    const Btrfs* btrfs = tmp->get_btrfs();
		    return make_pair(btrfs, CompoundAction::Impl::Type::BTRFS_QGROUPS);
		}
	    }

	    const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
	    if (delete_action && is_btrfs_qgroup(delete_action->get_device(actiongraph->get_impl())))
	    {
		const BtrfsQgroup* tmp = to_btrfs_qgroup(delete_action->get_device(actiongraph->get_impl()));
		const Btrfs* btrfs = tmp->get_btrfs();
		// redirect to RHS so that a combination of create and delete are shown as one compound action
		return make_pair(redirect_to(actiongraph->get_devicegraph(RHS), btrfs), CompoundAction::Impl::Type::BTRFS_QGROUPS);
	    }

	    const Action::SetLimits* set_limits_action = dynamic_cast<const Action::SetLimits*>(action);
	    if (set_limits_action)
	    {
		const BtrfsQgroup* tmp = to_btrfs_qgroup(set_limits_action->get_device(actiongraph->get_impl(), RHS));
		if (tmp->get_impl().has_btrfs_subvolume())
		{
		    const BtrfsSubvolume* btrfs_subvolume = tmp->get_impl().get_btrfs_subvolume();
		    return make_pair(btrfs_subvolume, CompoundAction::Impl::Type::NORMAL);
		}
		else
		{
		    const Btrfs* btrfs = tmp->get_btrfs();
		    return make_pair(btrfs, CompoundAction::Impl::Type::BTRFS_QGROUPS);
		}
	    }

	    return make_pair(CompoundAction::Impl::get_target_device(actiongraph, action), CompoundAction::Impl::Type::NORMAL);
	}

	if (action->affects_holder())
	{
	    const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	    if (create_action && is_btrfs_qgroup_relation(create_action->get_holder(actiongraph->get_impl())))
	    {
		const BtrfsQgroupRelation* tmp = to_btrfs_qgroup_relation(create_action->get_holder(actiongraph->get_impl()));
		const Btrfs* btrfs = tmp->get_btrfs();
		return make_pair(btrfs, CompoundAction::Impl::Type::BTRFS_QGROUPS);
	    }

	    const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
	    if (delete_action && is_btrfs_qgroup_relation(delete_action->get_holder(actiongraph->get_impl())))
	    {
		const BtrfsQgroupRelation* tmp = to_btrfs_qgroup_relation(delete_action->get_holder(actiongraph->get_impl()));
		const Btrfs* btrfs = tmp->get_btrfs();
		// redirect to RHS so that a combination of create and delete are shown as one compound action
		return make_pair(redirect_to(actiongraph->get_devicegraph(RHS), btrfs), CompoundAction::Impl::Type::BTRFS_QGROUPS);
	    }
	}

	ST_THROW(Exception("get_meta_device failed"));
    }


    shared_ptr<CompoundAction>
    CompoundAction::Generator::find_by_target_device(const vector<shared_ptr<CompoundAction>>& compound_actions,
						     const Device* device, CompoundAction::Impl::Type type) const
    {
	vector<shared_ptr<CompoundAction>>::const_iterator it =
	    find_if(compound_actions.begin(), compound_actions.end(),
		[device, type](shared_ptr<const CompoundAction> a) -> bool {
		    return a->get_target_device() == device && a->get_impl().get_type() == type;
		});

	if (it == compound_actions.end())
	    return nullptr;

	return *it;
    }

}
