/*
 * Copyright (c) [2017-2019] SUSE LLC
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
#include "storage/CompoundAction/Generator.h"
#include "storage/CompoundAction/Formatter/Bcache.h"
#include "storage/CompoundAction/Formatter/Btrfs.h"
#include "storage/CompoundAction/Formatter/BtrfsSubvolume.h"
#include "storage/CompoundAction/Formatter/LvmLv.h"
#include "storage/CompoundAction/Formatter/LvmVg.h"
#include "storage/CompoundAction/Formatter/Md.h"
#include "storage/CompoundAction/Formatter/Nfs.h"
#include "storage/CompoundAction/Formatter/Partition.h"
#include "storage/CompoundAction/Formatter/StrayBlkDevice.h"
#include "storage/ActiongraphImpl.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/Format.h"


namespace storage
{

    CompoundAction::Impl::Impl(const Actiongraph* actiongraph)
    : actiongraph(actiongraph), target_device(nullptr), commit_actions(0)
    {}


    CompoundAction::Impl::~Impl() {}


    const Actiongraph*
    CompoundAction::Impl::get_actiongraph() const
    {
	return actiongraph;
    }


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


    string
    CompoundAction::Impl::sentence() const
    {
	if (is_partition(target_device))
	    return CompoundAction::Formatter::Partition(this).string_representation();

	else if (is_stray_blk_device(target_device))
	    return CompoundAction::Formatter::StrayBlkDevice(this).string_representation();

	else if (is_lvm_lv(target_device))
	    return CompoundAction::Formatter::LvmLv(this).string_representation();

	else if (is_lvm_vg(target_device))
	    return CompoundAction::Formatter::LvmVg(this).string_representation();

	else if (is_btrfs(target_device))
	    return CompoundAction::Formatter::Btrfs(this).string_representation();

	else if (is_btrfs_subvolume(target_device))
	    return CompoundAction::Formatter::BtrfsSubvolume(this).string_representation();

	else if (is_nfs(target_device))
	    return CompoundAction::Formatter::Nfs(this).string_representation();

	else if (is_bcache(target_device) || is_bcache_cset(target_device))
	    return CompoundAction::Formatter::Bcache(this).string_representation();

	else if (is_md(target_device))
	    return CompoundAction::Formatter::Md(this).string_representation();

	else
	    return boost::algorithm::join(get_commit_actions_as_strings(), "\n");
    }


    bool
    CompoundAction::Impl::is_delete() const
    {
	for (auto action : commit_actions)
	{
	    if (storage::is_delete(action) && get_target_device(actiongraph, action) == target_device)
		return true;
	}

	return false;
    }


    // static methods


    const Device*
    CompoundAction::Impl::get_target_device(const Actiongraph* actiongraph, const Action::Base* action)
    {
	return get_target_device(device(actiongraph, action));
    }


    const Device*
    CompoundAction::Impl::get_target_device(const Device* device)
    {
	if (is_partition_table(device))
	    return get_target_device(to_partition_table(device));

	if (is_encryption(device))
	    return get_target_device(to_encryption(device));

	if (is_lvm_pv(device))
	    return get_target_device(to_lvm_pv(device));

	if (is_blk_filesystem(device))
	    return get_target_device(to_blk_filesystem(device));

	if (is_mount_point(device))
	    return get_target_device(to_mount_point(device));

	return device;
    }


    const Device*
    CompoundAction::Impl::get_target_device(const PartitionTable* partition_table)
    {
	return partition_table->get_partitionable();
    }


    const Device*
    CompoundAction::Impl::get_target_device(const Encryption* encryption)
    {
	return encryption->get_blk_device();
    }


    const Device*
    CompoundAction::Impl::get_target_device(const LvmPv* pv)
    {
	return get_target_device(pv->get_blk_device());
    }


    const Device*
    CompoundAction::Impl::get_target_device(const BlkFilesystem* blk_filesystem)
    {
	auto blk_devices = blk_filesystem->get_blk_devices();

	if (blk_devices.size() > 1)
	    // BtrFS with several devices
	    return blk_filesystem;

	return get_target_device(blk_devices.front());
    }


    const Device*
    CompoundAction::Impl::get_target_device(const MountPoint* mount_point)
    {
	return get_target_device(mount_point->get_mountable());
    }


    const Device*
    CompoundAction::Impl::device(const Actiongraph* actiongraph, const Action::Base* action)
    {
	if (storage::is_create(action))
	    return device(actiongraph, dynamic_cast<const Action::Create*>(action));

	else if (storage::is_modify(action))
	    return device(actiongraph, dynamic_cast<const Action::Modify*>(action));

	else if (storage::is_delete(action))
	    return device(actiongraph, dynamic_cast<const Action::Delete*>(action));

	else
	    ST_THROW(Exception("unknown commit action"));
    }


    const Device*
    CompoundAction::Impl::device(const Actiongraph* actiongraph, const Action::Create* action)
    {
	return action->get_device(actiongraph->get_impl());
    }


    const Device*
    CompoundAction::Impl::device(const Actiongraph* actiongraph, const Action::Modify* action)
    {
	try
	{
	    return action->get_device(actiongraph->get_impl(), RHS);

	}
	catch(const DeviceNotFound& e)
	{

	    return action->get_device(actiongraph->get_impl(), LHS);
	}
    }


    const Device*
    CompoundAction::Impl::device(const Actiongraph* actiongraph, const Action::Delete* action)
    {
	return action->get_device(actiongraph->get_impl());
    }


    CompoundAction*
    CompoundAction::Impl::find_by_target_device(Actiongraph* actiongraph, const Device* device)
    {
	return const_cast<CompoundAction*>(find_by_target_device(static_cast<const Actiongraph*>(actiongraph), device));
    }


    const CompoundAction*
    CompoundAction::Impl::find_by_target_device(const Actiongraph* actiongraph, const Device* device)
    {
	for (auto action : actiongraph->get_compound_actions())
	{
	    if (action->get_target_device() == device)
		return action;
	}

	ST_THROW(DeviceNotFound(sformat("target device not found, sid:%d", device->get_sid())));
    }

}

