/*
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_VIEW_H
#define STORAGE_VIEW_H


namespace storage
{

    /**
     * Enum with possible views on the devicegraph.
     */
    enum class View
    {
	/**
	 * All devices and holders are visible.
	 *
	 * The resulting devicegraph is a directed graph.
	 */
	ALL,

	/**
	 * The classical view. Devices of type BtrfsQgroup and holders of type Snapshot
	 * and BtrfsQgroupRelation are not visible.
	 *
	 * The resulting devicegraph is a directed acyclic graph.
	 */
	CLASSIC,

	/**
	 * A special view for removing devices.
	 *
	 * This view is useful when removing descendants. When removing a normal logical
	 * volume with snapshots the snapshots must also be removed. When removing a thin
	 * logical volume the snapshots can stay. This view implements this behaviour.
	 *
	 * The resulting devicegraph is a directed acyclic graph.
	 */
	REMOVE
    };

}


#endif
