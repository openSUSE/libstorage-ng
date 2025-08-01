/*
 * Copyright (c) [2010-2014] Novell, Inc.
 * Copyright (c) [2023-2025] SUSE LLC
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


#ifndef STORAGE_CMD_LSSCSI_H
#define STORAGE_CMD_LSSCSI_H


#include <string>
#include <map>
#include <vector>

#include "storage/Devices/Disk.h"


namespace storage
{
    using std::string;
    using std::map;
    using std::vector;


    class CmdLsscsi
    {
    public:

	CmdLsscsi();

	struct Entry
	{
	    Transport transport = Transport::UNKNOWN;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdLsscsi& cmd_lsscsi);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	bool get_entry(const string& device, Entry& entry) const;

    private:

	void parse(const vector<string>& lines);

	typedef map<string, Entry>::const_iterator const_iterator;

	map<string, Entry> data;

    };


    class CmdLsscsiVersion
    {
    public:

	static void query_version();
	static void parse_version(const string& version);

	static bool supports_json_option();

    private:

	static bool did_set_version;

	static int major;
	static int minor;

    };

}


#endif
