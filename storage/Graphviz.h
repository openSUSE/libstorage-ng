/*
 * Copyright (c) [2016,2018] SUSE LLC
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


#ifndef STORAGE_GRAPHVIZ_H
#define STORAGE_GRAPHVIZ_H


namespace storage
{

    /**
     * Bitfield to control graphviz output.
     *
     * Not all flags apply to all graphs or all objects (devices, holders and
     * actions).
     */
    enum class GraphvizFlags : unsigned int
    {
	NONE = 0,
	CLASSNAME = 1 << 0,
	PRETTY_CLASSNAME = 1 << 1,
	NAME = 1 << 2,
	SID = 1 << 3,
	SIZE = 1 << 4,
	ACTIVE = 1 << 5,
	IN_ETC = 1 << 6
    };


    GraphvizFlags operator|(GraphvizFlags a, GraphvizFlags b);
    GraphvizFlags operator&(GraphvizFlags a, GraphvizFlags b);

    bool operator&&(GraphvizFlags a, GraphvizFlags b);

}


#endif
