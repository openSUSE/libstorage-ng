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


#ifndef STORAGE_COMPOUND_ACTION_FORMATER_H
#define STORAGE_COMPOUND_ACTION_FORMATER_H


#include <string>

#include "storage/CompoundActionImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Utils/Text.h"


namespace storage
{

    class CompoundAction::Formater
    {

    public:

	class Btrfs;
	class BtrfsSubvolume;
	class BtrfsSubvolume;
	class LvmLv;
	class LvmVg;
	class Nfs;
	class Partition;

	Formater(const CompoundAction::Impl* compound_action);
	virtual ~Formater();

	std::string string_representation() const;

    private:

	virtual Text text() const = 0;

    protected:

	Text default_text() const;

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
		if (storage::is_delete(action) && is_device_of_type<const Type>(device))
		    return action;
	    }

	    return nullptr; 
	}


    protected:
    
	const CompoundAction::Impl* compound_action;

	Tense tense;
    
    };

}

#endif

