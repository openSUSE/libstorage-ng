/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#ifndef STORAGE_COMPOUND_ACTION_IMPL_H
#define STORAGE_COMPOUND_ACTION_IMPL_H


#include <vector>

#include "storage/CompoundAction.h"
#include "storage/Action.h"
#include "storage/Devices/Device.h"


namespace storage
{

    using std::vector;
    using std::string;

    class PartitionTable;
    class Encryption;
    class LvmPv;
    class BlkFilesystem;
    class MountPoint;


    class CompoundAction::Impl
    {
    public:

	enum class Type { NORMAL, BTRFS_QUOTA, BTRFS_QGROUPS };

	Impl(const Actiongraph* actiongraph);

	const Actiongraph* get_actiongraph() const { return actiongraph; }

	const Device* get_target_device() const { return target_device; }
	void set_target_device(const Device* target_device) { Impl::target_device = target_device; }

	Type get_type() const { return type; }
	void set_type(Type type) { Impl::type = type; }

	vector<const Action::Base*> get_commit_actions() const { return commit_actions; }

	void add_commit_action(const Action::Base* action);

    private:

	vector<string> get_commit_actions_as_strings() const;

    public:

	string sentence() const;

	bool is_delete() const;

	static const Device* get_target_device(const Actiongraph* actiongraph, const Action::Base* action);

    private:

	static const Device* get_target_device(const Device* device);
	static const Device* get_target_device(const PartitionTable* partition_table);
	static const Device* get_target_device(const Encryption* encryption);
	static const Device* get_target_device(const LvmPv* pv);
	static const Device* get_target_device(const BlkFilesystem* blk_filesystem);
	static const Device* get_target_device(const MountPoint* mount_point);

    public:

	static const Device* device(const Actiongraph* actiongraph, const Action::Base* action);
	static const Device* device(const Actiongraph* actiongraph, const Action::Create* action);
	static const Device* device(const Actiongraph* actiongraph, const Action::Modify* action);
	static const Device* device(const Actiongraph* actiongraph, const Action::Delete* action);

	static CompoundAction* find_by_target_device(Actiongraph* actiongraph, const Device* device);
	static const CompoundAction* find_by_target_device(const Actiongraph* actiongraph, const Device* device);

    private:

	const Actiongraph* actiongraph;

	const Device* target_device = nullptr;

	Type type = Type::NORMAL;

	vector<const Action::Base*> commit_actions;

    };

}

#endif
