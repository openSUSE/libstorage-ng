/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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


#ifndef STORAGE_ACTION_REALLOT_H
#define STORAGE_ACTION_REALLOT_H


#include "storage/Actions/Modify.h"
#include "storage/Devices/Device.h"


namespace storage
{

    /**
     * We use the term "reallot" for reducing or extending a container, e.g. a
     * LVM volume group, a MD RAID, or a btrfs.
     */
    enum class ReallotMode
    {
	REDUCE, EXTEND
    };


    namespace Action
    {

	class Reallot : public Modify
	{
	public:

	    Reallot(sid_t sid, ReallotMode reallot_mode, const Device* device)
		: Modify(sid), reallot_mode(reallot_mode), device(device) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;
	    virtual uf_t used_features(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	    const ReallotMode reallot_mode;

	    /**
	     * The device for addition or removal.
	     */
	    const Device* device;

	private:

	    /**
	     * Checks if the given action is a Reallot::REDUCE performed on the
	     * same device (PV) than this action.
	     */
	    bool action_removes_device(const Action::Base* action) const;

	};

    }

}

#endif
