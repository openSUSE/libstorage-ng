/*
 * Copyright (c) 2016 SUSE LLC
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


#include <type_traits>

#include "storage/Graphviz.h"


namespace storage
{

    GraphvizFlags
    operator|(GraphvizFlags a, GraphvizFlags b)
    {
	typedef std::underlying_type<GraphvizFlags>::type underlying_type;

	return static_cast<GraphvizFlags>(static_cast<underlying_type>(a) |
					  static_cast<underlying_type>(b));
    }


    GraphvizFlags
    operator&(GraphvizFlags a, GraphvizFlags b)
    {
	typedef std::underlying_type<GraphvizFlags>::type underlying_type;

	return static_cast<GraphvizFlags>(static_cast<underlying_type>(a) &
					  static_cast<underlying_type>(b));
    }


    bool
    operator&&(GraphvizFlags a, GraphvizFlags b)
    {
	return (a & b) != GraphvizFlags::NONE;
    }

}
