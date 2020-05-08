/*
 * Copyright (c) [2015-2017] SUSE LLC
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


#ifndef STORAGE_CMD_LSATTR_H
#define STORAGE_CMD_LSATTR_H


#include <string>
#include <vector>
#include <tuple>


namespace storage
{
    using std::string;
    using std::vector;


    class CmdLsattr
    {
    public:

	typedef std::tuple<string, string> key_t;

	CmdLsattr(const key_t& key, const string& mountpoint, const string& path);

	bool is_nocow() const { return nocow; }

	friend std::ostream& operator<<(std::ostream& s, const CmdLsattr& cmd_lsattr);

	void parse(const vector<string>& lines);

    private:

	string mountpoint;
	string path;

	bool nocow;

    };

}


#endif
