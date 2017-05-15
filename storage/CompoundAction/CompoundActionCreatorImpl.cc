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


#include <iostream>
#include <algorithm>

#include "storage/CompoundAction/CompoundActionCreatorImpl.h"
#include "storage/Actiongraph.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"


namespace storage
{

    using namespace std;

    using const_iterator = vector<CompoundAction*>::const_iterator;


    CompoundActionCreator::Impl::Impl(const Actiongraph* actiongraph)
    : actiongraph(actiongraph) {}


    CompoundActionCreator::Impl::~Impl()
    {
	for (auto compound_action : compound_actions)
	    delete compound_action;
    }


    vector<const Action::Base*>
    CompoundActionCreator::Impl::get_commit_actions() const
    {
	return actiongraph->get_commit_actions();
    }


    vector<CompoundAction*>
    CompoundActionCreator::Impl::get_compound_actions() const
    {
	return compound_actions;
    }


    //TODO Right now, delete actions are collected together to other
    // commit actions sharing the target device.
    // Delete actions should generate another separate compound action. 
    void
    CompoundActionCreator::Impl::group_commit_actions()
    {
	for(auto& commit_action : get_commit_actions())
	{
	    auto target = target_device(commit_action);
	    
	    auto compound_action = find_by_target_device(target);

	    if (compound_action == compound_actions.end())
		add_compound_action(commit_action);
	    else
		(*compound_action)->add_commit_action(commit_action);
	}
    }


    void
    CompoundActionCreator::Impl::add_compound_action(const Action::Base* commit_action)
    {
	auto target = target_device(commit_action);
	
	CompoundAction* compound_action = new CompoundAction(actiongraph);

	compound_action->set_target_device(target);
	compound_action->add_commit_action(commit_action);
	compound_actions.push_back(compound_action);
    }

    
    const_iterator
    CompoundActionCreator::Impl::find_by_target_device(const Device* device) const
    {
	auto begin = compound_actions.begin();
	auto end = compound_actions.end();

	auto it = find_if(begin, end, 
		[device](CompoundAction* a) -> bool 
		{
		    return a->get_target_device() == device;
		});

	return it;
    }


    const Device*
    CompoundActionCreator::Impl::target_device(const Action::Base* action) const
    {
	return target_device(device(action));
    }


    const Device*
    CompoundActionCreator::Impl::target_device(const Device* device) const
    {
	if (is_partition_table(device))
	    return target_device(to_partition_table(device));

	if (is_encryption(device))
	    return target_device(to_encryption(device));

	if (is_lvm_pv(device))
	    return target_device(to_lvm_pv(device));

	if (is_blk_filesystem(device))
	    return target_device(to_blk_filesystem(device));

	if (is_mount_point(device))
	    return target_device(to_mount_point(device));

	return device;
    }
    

    const Device*
    CompoundActionCreator::Impl::target_device(const PartitionTable* partition_table) const
    {
	return partition_table->get_partitionable();
    }


    const Device*
    CompoundActionCreator::Impl::target_device(const Encryption* encryption) const
    {
	return encryption->get_blk_device();
    }


    const Device*
    CompoundActionCreator::Impl::target_device(const LvmPv* pv) const
    {
	return target_device(pv->get_blk_device());
    }


    const Device*
    CompoundActionCreator::Impl::target_device(const BlkFilesystem* blk_filesystem) const
    {
	auto blk_devices = blk_filesystem->get_blk_devices(); 

	if (blk_devices.size() > 1)
	    // BtrFS with several devices
	    return blk_filesystem;
	    
	return target_device(blk_devices.front());
    }


    const Device*
    CompoundActionCreator::Impl::target_device(const MountPoint* mount_point) const
    {
	return target_device(mount_point->get_mountable());
    }


    const Device*
    CompoundActionCreator::Impl::device(const Action::Base* action) const
    {
	if (is_create(action))
	    return device(dynamic_cast<const Action::Create*>(action));

	if (is_modify(action))
	    return device(dynamic_cast<const Action::Modify*>(action)); 

	if (is_delete(action))
	    return device(dynamic_cast<const Action::Delete*>(action));
    }


    const Device*
    CompoundActionCreator::Impl::device(const Action::Create* action) const
    {
	return action->get_device(actiongraph->get_impl());
    }


    const Device*
    CompoundActionCreator::Impl::device(const Action::Modify* action) const
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
    CompoundActionCreator::Impl::device(const Action::Delete* action) const
    {
	return action->get_device(actiongraph->get_impl());
    }

}

