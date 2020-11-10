/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#ifndef STORAGE_COMPOUND_ACTION_GENERATOR_H
#define STORAGE_COMPOUND_ACTION_GENERATOR_H


#include <vector>

#include "storage/CompoundActionImpl.h"


namespace storage
{
    using namespace std;


    class Actiongraph;
    class Device;

    class CompoundAction::Generator
    {

    public:

	Generator(const Actiongraph* actiongraph);

	vector<shared_ptr<CompoundAction>> generate() const;

    private:

	pair<const Device*, CompoundAction::Impl::Type> get_meta_device(const Action::Base* action) const;

	CompoundAction* find_by_target_device(const vector<CompoundAction*>& compound_actions,
					      const Device* device, CompoundAction::Impl::Type type) const;

	const Actiongraph* actiongraph = nullptr;

    };

}

#endif
