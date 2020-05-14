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


#include <map>
#include <functional>

#include "storage/Devicegraph.h"


namespace storage
{
    using namespace std;


    typedef std::function<Device* (Devicegraph* devicegraph, const xmlNode* node)> device_load_fnc;

    typedef std::function<Holder* (Devicegraph* devicegraph, const xmlNode* node)> holder_load_fnc;

    /**
     * Map with name of all non-abstract device types and corresponding load function.
     */
    extern const map<string, device_load_fnc> device_load_registry;

    /**
     * Map with name of all non-abstract holder types and corresponding load function.
     */
    extern const map<string, holder_load_fnc> holder_load_registry;

}
