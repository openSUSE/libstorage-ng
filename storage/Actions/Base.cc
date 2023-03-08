/*
 * Copyright (c) 2023 SUSE LLC
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


#include "storage/Actions/BaseImpl.h"
#include "storage/ActiongraphImpl.h"


namespace storage
{

    using namespace std;


    string
    get_string(const Actiongraph* actiongraph, const Action::Base* action)
    {
	const CommitData commit_data(actiongraph->get_impl(), Tense::SIMPLE_PRESENT);

	return action->text(commit_data).translated;
    }

}
