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


#ifndef STORAGE_ACTION_MODIFY_IMPL_H
#define STORAGE_ACTION_MODIFY_IMPL_H


#include "storage/Actions/BaseImpl.h"
#include "storage/Actions/Modify.h"


namespace storage
{

    namespace Action
    {

	class Modify : public Base
	{
	public:

	    Modify(sid_t sid, bool only_sync = false) : Base(sid, only_sync) {}

	    /**
	     * Returns the device of the action on the LHS or RHS devicegraph. Only valid
	     * for actions affecting a device. May not exist.
	     */
	    Device* get_device(const Actiongraph::Impl& actiongraph, Side side) const;

	};

    }


    static_assert(std::is_abstract<Action::Modify>(), "Modify ought to be abstract.");

}

#endif
