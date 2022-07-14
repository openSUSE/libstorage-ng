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


#ifndef STORAGE_MOUNT_POINT_PATH_H
#define STORAGE_MOUNT_POINT_PATH_H


#include <string>


namespace storage
{

    using std::string;


    /**
     * Just the path of a mount point and a rootprefixed flag.
     */
    class MountPointPath
    {
    public:

	MountPointPath(const string& path, bool rootprefixed)
	    : path(path), rootprefixed(rootprefixed) {}

	MountPointPath(const string& fullpath, const string& rootprefix);

	string path;
	bool rootprefixed;

	string fullpath(const string& rootprefix) const;

	bool operator==(const MountPointPath& rhs) const;
	bool operator!=(const MountPointPath& rhs) const { return !(*this == rhs); }

    };

}

#endif
