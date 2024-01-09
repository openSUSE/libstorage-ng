/*
 * Copyright (c) [2018-2024] SUSE LLC
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


#ifndef STORAGE_LIGHT_PROBE_H
#define STORAGE_LIGHT_PROBE_H


namespace storage
{

    class SystemInfo;


    /**
     * This function does some very light probing and returns true iff some
     * disks or DASDs were found in the system. No devicegraph is generated.
     * The exact type of devices probed may change.
     *
     * @throw Exception
     */
    bool light_probe();


    /**
     * Same as light_probe() except for the SystemInfo parameter.
     *
     * @see light_probe()
     *
     * @throw Exception
     */
    bool light_probe(SystemInfo& system_info);

}


#endif
