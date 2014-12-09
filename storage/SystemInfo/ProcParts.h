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


#ifndef PROC_PARTS_H
#define PROC_PARTS_H


#include <string>
#include <list>
#include <map>
#include <vector>


namespace storage_bgl
{
    using std::string;
    using std::list;
    using std::map;
    using std::vector;


    class ProcParts
    {
    public:

	ProcParts(bool do_probe = true);

	void probe();

	bool getSize(const string& device, unsigned long long& sizeK) const;
	bool findDevice(const string& device) const;

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

	friend std::ostream& operator<<(std::ostream& s, const ProcParts& procparts);

	void parse(const vector<string>& lines);

    private:

	typedef map<string, unsigned long long>::const_iterator const_iterator;

	const_iterator findEntry(const string& device) const;

	map<string, unsigned long long> data;

    };

}


#endif
