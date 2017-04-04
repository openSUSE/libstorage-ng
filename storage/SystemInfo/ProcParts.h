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


#ifndef STORAGE_PROC_PARTS_H
#define STORAGE_PROC_PARTS_H


#include <string>
#include <map>
#include <vector>


namespace storage
{
    using std::string;
    using std::map;
    using std::vector;


    class ProcParts
    {
    public:

	ProcParts();

	bool get_size(const string& device, unsigned long long& size) const;
	bool find_device(const string& device) const;

	vector<string> get_entries() const;

	template<class Pred>
	vector<string> get_matching_entries(Pred pred) const
	{
	    vector<string> ret;
	    for (const_iterator i = data.begin(); i != data.end(); ++i)
		if (pred(i->first))
		    ret.push_back(i->first);
	    return ret;
	}

	typedef map<string, unsigned long long>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	friend std::ostream& operator<<(std::ostream& s, const ProcParts& proc_parts);

    private:

	void parse(const vector<string>& lines);

	const_iterator find_entry(const string& device) const;

	map<string, unsigned long long> data;

    };

}


#endif
