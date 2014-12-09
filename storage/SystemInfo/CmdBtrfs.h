/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#ifndef CMD_BTRFS_H
#define CMD_BTRFS_H


#include <vector>
#include <list>
#include <map>


namespace storage_bgl
{
    using std::vector;
    using std::list;
    using std::map;


    class CmdBtrfsShow
    {
    public:

	CmdBtrfsShow(bool do_probe = true);

	void probe();

	struct Entry
	{
	    list<string> devices;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdBtrfsShow& cmdbtrfsshow);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	bool getEntry(const string& uuid, Entry& entry) const;

	list<string> getUuids() const;

	void parse(const vector<string>& lines);

    private:

	typedef map<string, Entry>::const_iterator const_iterator;

	map<string, Entry> data;

    };

}

#endif
