/*
 * Copyright (c) [2017-2019] SUSE LLC
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


#include "storage/CompoundActionImpl.h"


namespace storage
{

    CompoundAction::CompoundAction(const Actiongraph* actiongraph)
	: impl(make_unique<Impl>(actiongraph))
    {
    }


    CompoundAction::~CompoundAction() {}


    const Device*
    CompoundAction::get_target_device() const
    {
	return get_impl().get_target_device();
    }


    string
    CompoundAction::sentence() const
    {
	return get_impl().sentence();
    }


    bool
    CompoundAction::is_delete() const
    {
	return get_impl().is_delete();
    }


    CompoundAction*
    CompoundAction::find_by_target_device(Actiongraph* actiongraph, const Device* device)
    {
	return CompoundAction::Impl::find_by_target_device(actiongraph, device);
    }


    const CompoundAction*
    CompoundAction::find_by_target_device(const Actiongraph* actiongraph, const Device* device)
    {
	return CompoundAction::Impl::find_by_target_device(actiongraph, device);
    }

}
