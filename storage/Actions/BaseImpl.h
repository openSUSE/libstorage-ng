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


#ifndef STORAGE_ACTIONS_BASE_IMPL_H
#define STORAGE_ACTIONS_BASE_IMPL_H


#include "storage/Utils/ExceptionImpl.h"
#include "storage/Devices/Device.h"
#include "storage/ActiongraphImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/CommitOptions.h"


namespace storage
{

    namespace Action
    {

	/**
	 * Abstract base class for actions. An action can either affect a device or a
	 * holder. Thus either sid or sid_pair is valid.
	 */
	class Base : private boost::noncopyable
	{
	public:

	    enum class Affect { DEVICE, HOLDER };

	    Base(sid_t sid, bool only_sync, bool nop = false)
		: affect(Affect::DEVICE), sid(sid), only_sync(only_sync), nop(nop) {}

	    Base(sid_pair_t sid_pair, bool only_sync, bool nop = false)
		: affect(Affect::HOLDER), sid_pair(sid_pair), only_sync(only_sync), nop(nop) {}

	    virtual ~Base() {}

	    virtual Text text(const CommitData& commit_data) const = 0;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const = 0;
	    virtual uf_t used_features(const Actiongraph::Impl& actiongraph) const { return 0; }

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const {}

	    /**
	     * Returns a string representing some information, sid or sid_pair and some
	     * flags, of the action.
	     */
	    string details() const;

	    /**
	     * Returns the native message plus nop indication. Useful for debugging and testsuites.
	     */
	    string debug_text(const CommitData& commit_data) const;

	    const Affect affect;

	    bool affects_device() const { return affect == Affect::DEVICE; }
	    bool affects_holder() const { return affect == Affect::HOLDER; }

	    union
	    {
		// Having members with non-trivial special member function causes those
		// functions to be deleted for the union since they would be
		// ill-defined. But apart from that it seems to be fine (since C++11).

		const sid_t sid;
		const sid_pair_t sid_pair;
	    };

	    bool first = false;
	    bool last = false;

	    // Action is only used as interim synchronization point and will be removed.
	    bool only_sync = false;

	    // Action is only used to inform user but does no operation.
	    bool nop = false;

	};

    }


    template <typename Type>
    bool
    is_action_of_type(const Action::Base* action)
    {
	static_assert(std::is_const<Type>::value, "Type must be const");

	ST_CHECK_PTR(action);

	return dynamic_cast<const Type*>(action);
    }


    static_assert(std::is_abstract<Action::Base>(), "Base ought to be abstract.");

}

#endif
