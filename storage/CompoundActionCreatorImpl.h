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


#ifndef STORAGE_COMPOUND_ACTION_CREATOR_IMPL_H
#define STORAGE_COMPOUND_ACTION_CREATOR_IMPL_H


#include <iterator>

#include "storage/CompoundActionCreator.h"
#include "storage/Action.h"
#include "storage/Devices/Device.h"


namespace storage
{

    using std::vector;

    class PartitionTable;
    class Encryption;
    class LvmPv;
    class BlkFilesystem;
    class MountPoint;

    class CompoundActionCreator::Impl
    {

    public:

	using const_iterator = vector<CompoundAction*>::const_iterator;

	Impl(const Actiongraph* actiongraph);
	~Impl();

	vector<const Action::Base*> get_commit_actions() const;

	vector<CompoundAction*> get_compound_actions() const;

	void group_commit_actions();
	
	void add_compound_action(const Action::Base* action);

	const_iterator find_by_target_device(const Device* device) const;

	const Device* target_device(const Action::Base* action) const;
	const Device* target_device(const Device* device) const;
	const Device* target_device(const PartitionTable* partition_table) const;
	const Device* target_device(const Encryption* encryption) const;
	const Device* target_device(const LvmPv* pv) const;
	const Device* target_device(const BlkFilesystem* blk_filesystem) const;
	const Device* target_device(const MountPoint* mount_point) const;

	const Device* device(const Action::Base* action) const;
	const Device* device(const Action::Create* action) const;
	const Device* device(const Action::Modify* action) const;
	const Device* device(const Action::Delete* action) const;

    private:

	const Actiongraph* actiongraph;
    
	vector<CompoundAction*> compound_actions;

    };

}

#endif
