/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) 2017 SUSE LLC
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


#ifndef STORAGE_CMD_DMRAID_H
#define STORAGE_CMD_DMRAID_H


#include <string>
#include <vector>
#include <map>


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


    class CmdDmraid
    {

    public:

	CmdDmraid();

	struct Entry
	{
	    string raid_type;
	    string controller;
	    vector<string> devices;
	};

	vector<string> get_entries() const;

	bool get_entry(const string& name, Entry& entry) const;

	typedef map<string, Entry>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	friend std::ostream& operator<<(std::ostream& s, const CmdDmraid& cmd_dmraid);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

    private:

	void parse(const vector<string>& lines);

	map<string, Entry> data;

    };

}

#endif
