/*
 * Copyright (c) [2004-2009] Novell, Inc.
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


#ifndef CMD_MULTIPATH_H
#define CMD_MULTIPATH_H


#include <string>
#include <list>
#include <map>


namespace storage
{
    using std::string;
    using std::list;
    using std::map;


    class CmdMultipath
    {

    public:

	CmdMultipath(bool test = false);

	struct Entry
	{
	    string vendor;
	    string model;
	    list<string> devices;
	};

	list<string> getEntries() const;

	bool getEntry(const string& name, Entry& entry) const;

	bool looksLikeRealMultipath() const;

    private:

	typedef map<string, Entry>::const_iterator const_iterator;

	map<string, Entry> data;

    };

}

#endif
