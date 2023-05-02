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


#ifndef STORAGE_ACTION_BASE_H
#define STORAGE_ACTION_BASE_H


#include <string>


namespace storage
{

    class Actiongraph;

    namespace Action
    {

	class Base;

    }


    /**
     * A color suggestion for commit messages.
     *
     * In general the color is green for action that create, mount or activate something,
     * red for action that delete, unmount or deactivate something and black otherwise.
     *
     * The color of an action is not set in stone and may change in the future. It must
     * only be used to displaying messages. Not for any other logic!
     *
     * @see get_color()
     */
    enum class Color
    {
	GREEN, BLACK, RED
    };


    /**
     * Get the commit message of an action.
     *
     * TODO: This should be a member function of Action::Base but that is not available in
     * the API in all details.
     */
    std::string
    get_string(const Actiongraph* actiongraph, const Action::Base* action);


    /**
     * Get the color for the commit message of an action.
     *
     * @see Color
     */
    Color
    get_color(const Actiongraph* actiongraph, const Action::Base* action);

}

#endif
