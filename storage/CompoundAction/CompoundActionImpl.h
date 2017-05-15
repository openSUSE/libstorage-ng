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


#ifndef STORAGE_COMPOUND_ACTION_IMPL_H
#define STORAGE_COMPOUND_ACTION_IMPL_H


#include "storage/CompoundAction/CompoundAction.h"


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

	Impl(const Actiongraph* actiongraph);
	Impl(const Actiongraph* actiongraph, const Action::Base* action);
	~Impl();

	const Actiongraph* get_actiongraph() const;

	void set_target_device(const Device* device);

	const Device* get_target_device() const;

	void set_commit_actions(vector<const Action::Base*> actions);

	vector<const Action::Base*> get_commit_actions() const; 

	void add_commit_action(const Action::Base* action);
    
	vector<string> get_commit_actions_as_strings() const;

	static vector<CompoundAction*> generate(const Actiongraph* actiongraph);

	static const Device* get_target_device(const Actiongraph* actiongraph, const Action::Base* action);
	static const Device* get_target_device(const Device* device);
	static const Device* get_target_device(const PartitionTable* partition_table);
	static const Device* get_target_device(const Encryption* encryption);
	static const Device* get_target_device(const LvmPv* pv);
	static const Device* get_target_device(const BlkFilesystem* blk_filesystem);
	static const Device* get_target_device(const MountPoint* mount_point);

	static const Device* device(const Actiongraph* actiongraph, const Action::Base* action);
	static const Device* device(const Actiongraph* actiongraph, const Action::Create* action);
	static const Device* device(const Actiongraph* actiongraph, const Action::Modify* action);
	static const Device* device(const Actiongraph* actiongraph, const Action::Delete* action);


	//bool is_delete() const;

	string to_string() const;
	
//	string to_string(const Partition* partition) const;
//	Text delete_partition_text(const Partition* partition, Tense tense) const;
//	Text create_partition_text(const Partition* partition, Tense tense) const;
//	Text create_encrypted_pv_partition_text(const Partition* partition, Tense tense) const;
//	Text create_pv_partition_text(const Partition* partition, Tense tense) const;
//	Text create_encrypted_plane_partition_text(const Partition* partition, Tense tense) const;
//	Text create_plane_partition_text(const Partition* partition, Tense tense) const;
//	
//	bool has_encryption() const;
//	bool has_pv() const;

	//string to_string(const LvmLv* lv) const;
	//Text create_lvm_lv_text(const LvmLv* lv, Tense tense) const;

    private:
	
	const Actiongraph* actiongraph;

	const Device* target_device;

	vector<const Action::Base*> commit_actions;
    
    };

}

#endif

