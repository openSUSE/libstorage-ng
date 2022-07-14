/*
 * Copyright (c) 2022 SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "MountPointPath.h"


namespace storage
{

    MountPointPath::MountPointPath(const string& fullpath, const string& rootprefix)
	: path(fullpath), rootprefixed(true)
    {
	if (path == "swap")
	    return;

	if (rootprefix.empty() || rootprefix == "/")
	    return;

	rootprefixed = boost::starts_with(path, rootprefix);

	if (rootprefixed)
	{
	    path.erase(0, rootprefix.size());
	    if (path.empty())
		path = "/";
	}
    }


    bool
    MountPointPath::operator==(const MountPointPath& rhs) const
    {
	return path == rhs.path && rootprefixed == rhs.rootprefixed;
    }


    string
    MountPointPath::fullpath(const string& rootprefix) const
    {
	if (!rootprefixed)
	    return path;

	if (path == "swap")
	    return path;

	if (rootprefix.empty() || rootprefix == "/")
	    return path;

	if (path == "/")
	    return rootprefix;
	else
	    return rootprefix + path;
    }

}
