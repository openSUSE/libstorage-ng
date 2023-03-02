/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_ACTION_DELETE_H
#define STORAGE_ACTION_DELETE_H


#include "storage/Actions/Base.h"


namespace storage
{

    namespace Action
    {

	class Delete : public Base
	{
	public:

	    Delete(sid_t sid, bool only_sync = false, bool nop = false)
		: Base(sid, only_sync, nop) {}

	    Delete(sid_pair_t sid_pair, bool only_sync = false, bool nop = false)
		: Base(sid_pair, only_sync, nop) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;
	    virtual uf_t used_features(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	    /**
	     * Returns the device of the action on the LHS devicegraph. Only valid for
	     * actions affecting a device.
	     */
	    Device* get_device(const Actiongraph::Impl& actiongraph) const;

	    /**
	     * Returns the holder of the action on the LHS devicegraph. Only valid for
	     * actions affecting a holder.
	     */
	    Holder* get_holder(const Actiongraph::Impl& actiongraph) const;

	private:

	    void add_device_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					 Actiongraph::Impl& actiongraph) const;

	    void add_holder_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					 Actiongraph::Impl& actiongraph) const;

	};

    }


    inline bool
    is_delete(const Action::Base* action)
    {
	return is_action_of_type<const Action::Delete>(action);
    }

}

#endif
