/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2017-2022] SUSE LLC
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


#ifndef STORAGE_PROC_MDSTAT_H
#define STORAGE_PROC_MDSTAT_H


#include <string>
#include <vector>
#include <map>

#include "storage/Devices/Md.h"


namespace storage
{
    using std::string;
    using std::map;
    using std::vector;


    class ProcMdstat
    {
    public:

	ProcMdstat();


	struct Device
	{
	    Device(const string& name, bool spare, bool faulty, bool journal)
		: name(name), spare(spare), faulty(faulty), journal(journal) {}

	    string name;
	    bool spare;
	    bool faulty;
	    bool journal;

	    bool operator<(const Device& rhs) const { return name < rhs.name; }

	};


	struct Entry
	{
	    MdLevel md_level = MdLevel::UNKNOWN;
	    MdParity md_parity = MdParity::DEFAULT;

	    string super;

	    unsigned long long size = 0;
	    unsigned long long chunk_size = 0;

	    bool read_only = false;
	    bool inactive = false;

	    vector<Device> devices;

	    bool is_container = false;

	    bool has_container = false;
	    string container_name;
	    string container_member;
	};

	friend std::ostream& operator<<(std::ostream& s, const ProcMdstat& proc_mdstat);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);
	friend std::ostream& operator<<(std::ostream& s, const Device& device);

	vector<string> get_entries() const;

	bool has_entry(const string& name) const;

	const Entry& get_entry(const string& name) const;

	typedef map<string, Entry>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

    private:

	void parse(const vector<string>& lines);

	Entry parse_entry(const string& line1, const string& line2) const;

	map<string, Entry> data;

    };

}


#endif
