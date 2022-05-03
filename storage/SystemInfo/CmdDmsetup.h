/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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


#ifndef STORAGE_CMD_DMSETUP_H
#define STORAGE_CMD_DMSETUP_H


#include <string>
#include <vector>
#include <map>


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


    class CmdDmsetupInfo
    {

    public:

	CmdDmsetupInfo();

	struct Entry
	{
	    dev_t majorminor = 0;
	    unsigned segments = 0;
	    string subsystem;
	    string uuid;
	};

	typedef map<string, Entry>::value_type value_type;
	typedef map<string, Entry>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	bool exists(const string& name) const;

	friend std::ostream& operator<<(std::ostream& s, const CmdDmsetupInfo& cmd_dmsetup_info);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

    private:

	void parse(const vector<string>& lines);

	map<string, Entry> data;

    };


    class CmdDmsetupTable
    {

    public:

	CmdDmsetupTable();

	struct Table
	{
	    Table(const string& target)
		: target(target) {}

	    string target;

	    /**
	     * stripes and stripe_size are valid iff target == striped.
	     */
	    unsigned long stripes = 0;
	    unsigned long long stripe_size = 0;

	    vector<dev_t> majorminors;
	};

	vector<Table> get_tables(const string& name) const;

	typedef map<string, vector<Table>>::value_type value_type;
	typedef map<string, vector<Table>>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	/**
	 * Find table using the device with major and minor number.
	 */
	const_iterator find_using(dev_t majorminor) const;

	friend std::ostream& operator<<(std::ostream& s, const CmdDmsetupTable& cmd_dmsetup_table);
	friend std::ostream& operator<<(std::ostream& s, const Table& table);

    private:

	void parse(const vector<string>& lines);

	map<string, vector<Table>> data;

    };

}

#endif
