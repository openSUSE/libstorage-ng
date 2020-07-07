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


#ifndef STORAGE_COMPOUND_ACTION_FORMATTER_H
#define STORAGE_COMPOUND_ACTION_FORMATTER_H


#include <string>

#include "storage/CompoundActionImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Devices/Encryption.h"
#include "storage/Utils/Text.h"

using std::string;


namespace storage
{

    class CompoundAction::Formatter
    {

    public:

	class Bcache;
	class Btrfs;
	class BtrfsSubvolume;
	class LvmLv;
	class LvmVg;
	class Nfs;
        class Md;
	class Partition;
	class StrayBlkDevice;

	Formatter(const CompoundAction::Impl* compound_action,
		  const string & device_classname = string() );

	virtual ~Formatter() {}

	string string_representation() const;

    private:

	virtual Text text() const = 0;

    protected:

	const BlkFilesystem* get_created_filesystem() const;

	const MountPoint* get_created_mount_point() const;

	const char * get_device_classname() const;

	Text default_text() const;

	// See also the template has_create()
	bool has_create(const string &classname) const;

	// See also the template has_delete()
	bool has_delete(const string &classname) const;

	template <typename Type>
	bool has_action() const
	{
	    return get_action<const Type>();
	}


	template <typename Type>
	const Action::Base* get_action() const
	{
	    for (auto action : _compound_action->get_commit_actions())
	    {
		if (is_action_of_type<const Type>(action))
		    return action;
	    }

	    return nullptr;
	}


	// See also has_create(const string&)
	template <typename Type>
	bool has_create() const
	{
	    return get_create<const Type>();
	}


	template <typename Type>
	const Action::Base* get_create() const
	{
	    for (auto action : _compound_action->get_commit_actions())
	    {
		auto device = CompoundAction::Impl::device(_compound_action->get_actiongraph(), action);
		if (is_create(action) && is_device_of_type<const Type>(device))
		    return action;
	    }

	    return nullptr;
	}


	// See also has_delete(const string&)
	template <typename Type>
	bool has_delete() const
	{
	    return get_delete<const Type>();
	}


	template <typename Type>
	const Action::Base* get_delete() const
	{
	    for (auto action : _compound_action->get_commit_actions())
	    {
		auto device = CompoundAction::Impl::device(_compound_action->get_actiongraph(), action);
		if (storage::is_delete(action) && is_device_of_type<const Type>(device))
		    return action;
	    }

	    return nullptr;
	}


	// Predicates

	virtual bool creating()	  const { return _creating;   }
	virtual bool deleting()	  const { return _deleting;   }
	virtual bool encrypting() const { return _encrypting; }
	virtual bool formatting() const { return _formatting; }
	virtual bool mounting()	  const { return _mounting;   }

	// Getters

	string get_mount_point()     const { return get_created_filesystem()->get_mount_point()->get_path(); }
	string get_filesystem_type() const { return get_created_filesystem()->get_displayname(); }

    protected:

	const CompoundAction::Impl* _compound_action;
	string  _device_classname;
	bool	_creating;
	bool	_deleting;
	bool	_encrypting;
	bool	_formatting;
	bool	_mounting;
    };

}

#endif
