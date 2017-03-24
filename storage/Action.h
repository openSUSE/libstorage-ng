/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_ACTION_H
#define STORAGE_ACTION_H


#include "storage/Utils/ExceptionImpl.h"
#include "storage/Devices/Device.h"
#include "storage/ActiongraphImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    class BlkDevice;


    namespace Action
    {

	class Base
	{
	public:

	    Base(sid_t sid, bool only_sync) : sid(sid), first(false), last(false), only_sync(only_sync) {}

	    virtual ~Base() {}

	    virtual Text text(const CommitData& commit_data) const = 0;
	    virtual void commit(CommitData& commit_data) const = 0;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const {}

	    const sid_t sid;

	    bool first;
	    bool last;

	    // Action is only used as interim synchronization point and will be removed.
	    bool only_sync;

	};


	class Create : public Base
	{
	public:

	    Create(sid_t sid, bool only_sync = false) : Base(sid, only_sync) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	    /**
	     * Returns the device of the action on the RHS devicegraph.
	     */
	    const Device* get_device(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(RHS)->find_device(sid); }

	};


	class Modify : public Base
	{
	public:

	    Modify(sid_t sid, bool only_sync = false) : Base(sid, only_sync) {}

	    /**
	     * Returns the device of the action on the Lhs or RHS devicegraph. May not exist.
	     */
	    const Device* get_device(const Actiongraph::Impl& actiongraph, Side side) const
		{ return actiongraph.get_devicegraph(side)->find_device(sid); }

	};


	/**
	 * Base class for renaming a blk device in some file, e.g. /etc/fstab
	 * or /etc/crypttab.
	 */
	class RenameIn : public Modify
	{
	public:

	    RenameIn(sid_t sid) : Modify(sid) {}

	    virtual const BlkDevice* get_renamed_blk_device(const Actiongraph::Impl& actiongraph,
							    Side side) const = 0;

	};


	class Delete : public Base
	{
	public:

	    Delete(sid_t sid, bool only_sync = false) : Base(sid, only_sync) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	    /**
	     * Returns the device of the action on the LHS devicegraph.
	     */
	    const Device* get_device(const Actiongraph::Impl& actiongraph) const
		{ return actiongraph.get_devicegraph(LHS)->find_device(sid); }

	};

    }


    template <typename Type>
    bool is_action_of_type(const Action::Base* action)
    {
	static_assert(std::is_const<Type>::value, "Type must be const");

	ST_CHECK_PTR(action);

	return dynamic_cast<const Type*>(action);
    }


    inline bool
    is_create(const Action::Base* action)
    {
	return is_action_of_type<const Action::Create>(action);
    }


    inline bool
    is_modify(const Action::Base* action)
    {
	return is_action_of_type<const Action::Modify>(action);
    }


    inline bool
    is_delete(const Action::Base* action)
    {
	return is_action_of_type<const Action::Delete>(action);
    }

}

#endif
