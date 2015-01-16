/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#include <string>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "storage/Graph.h"
#include "storage/Utils/AppUtil.h"
#include "storage/HumanString.h"
#include "storage/StorageInterface.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"


namespace storage_legacy
{
    using namespace std;
    using namespace storage;


    bool
    saveDeviceGraph(StorageInterface* s, const string& filename)
    {
	const Devicegraph* current = s->getStorage()->get_current();

	current->write_graphviz(filename);

	return true;
    }


    bool
    saveMountGraph(StorageInterface* s, const string& filename)
    {
	return false;
    }

}
