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


#ifndef STORAGE_PARTITION_FORMATER_H
#define STORAGE_PARTITION_FORMATER_H


#include <string>

#include "storage/CompoundAction/CompoundActionImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Utils/Text.h"

namespace storage
{

    class PartitionFormater
    {

    public:

	PartitionFormater(const CompoundAction::Impl* compound_action);
	~PartitionFormater();

	std::string string_representation() const;

    private:

	const BlkFilesystem* get_created_filesystem() const;

	Text text() const;

	Text delete_text() const;

	Text create_encrypted_pv_text() const;
	Text create_pv_text() const;
	Text encrypted_pv_text() const;
	Text pv_text() const;
	
	Text create_encrypted_with_fs_and_mount_point_text() const;
	Text create_encrypted_with_fs_text() const;
	Text create_encrypted_text() const;
	Text create_with_fs_and_mount_point_text() const;
	Text create_with_fs_text() const;
	Text create_text() const;
	Text encrypted_with_fs_and_mount_point_text() const;
	Text encrypted_with_fs_text() const;
	Text encrypted_text() const;
	Text fs_and_mount_point_text() const;
	Text fs_text() const;
	Text mount_point_text() const;


	template <typename Type>
	bool has_action() const
	{
	    return get_action<const Type>();
	}


	template <typename Type>
	const Action::Base* get_action() const
	{
	    for(auto action : compound_action->get_commit_actions())
	    {
		if (is_action_of_type<const Type>(action))
		    return action;
	    }

	    return nullptr; 
	}


	template <typename Type>
	bool has_create() const
	{
	    return get_create<const Type>();
	}


	template <typename Type>
	const Action::Base* get_create() const
	{
	    for(auto action : compound_action->get_commit_actions())
	    {
		auto device = CompoundAction::Impl::device(compound_action->get_actiongraph(), action);
		if (is_create(action) && is_device_of_type<const Type>(device))
		    return action;
	    }

	    return nullptr;
	}


	template <typename Type>
	bool has_delete() const
	{
	    return get_delete<const Type>();
	}


	template <typename Type>
	const Action::Base* get_delete() const
	{
	    for(auto action : compound_action->get_commit_actions())
	    {
		auto device = CompoundAction::Impl::device(compound_action->get_actiongraph(), action);
		if (is_delete(action) && is_device_of_type<const Type>(device))
		    return action;
	    }

	    return nullptr; 
	}


    private:
    
	const CompoundAction::Impl* compound_action;

	const Partition* partition;

	Tense tense;
    
    };

}

#endif

