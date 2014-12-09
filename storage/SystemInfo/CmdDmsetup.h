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


#ifndef CMD_DMSETUP_H
#define CMD_DMSETUP_H


#include <string>
#include <vector>
#include <list>
#include <map>


namespace storage_bgl
{
    using std::string;
    using std::vector;
    using std::list;
    using std::map;


    class CmdDmsetupInfo
    {

    public:

	CmdDmsetupInfo(bool do_probe = true);

	void probe();

	struct Entry
	{
	    Entry() : mjr(0), mnr(0), segments(0), uuid() {}

	    unsigned long mjr;
	    unsigned long mnr;
	    unsigned segments;
	    string uuid;
	};

	bool getEntry(const string& name, Entry& entry) const;

	list<string> getEntries() const;

	template<class Pred>
	list<string> getMatchingEntries(Pred pred) const
	{
	    list<string> ret;
	    for (const_iterator i = data.begin(); i != data.end(); ++i)
		if (pred(i->first))
		    ret.push_back(i->first);
	    return ret;
	}

	typedef map<string, Entry>::value_type value_type;
	typedef map<string, Entry>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	friend std::ostream& operator<<(std::ostream& s, const CmdDmsetupInfo& cmddmsetupinfo);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	void parse(const vector<string>& lines);

    private:

	map<string, Entry> data;

    };

}

#endif
