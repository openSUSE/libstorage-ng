/*
 * Copyright (c) [2018-2020] SUSE LLC
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


#ifndef STORAGE_CMD_STAT_H
#define STORAGE_CMD_STAT_H


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;


    class CmdStat
    {
    public:

	CmdStat(const string& path);

	bool is_blk() const { return S_ISBLK(mode); }
	bool is_dir() const { return S_ISDIR(mode); }
	bool is_reg() const { return S_ISREG(mode); }
	bool is_lnk() const { return S_ISLNK(mode); }

	friend std::ostream& operator<<(std::ostream& s, const CmdStat& cmd_stat);

    private:

	void parse(const vector<string>& lines);

	string path;

	mode_t mode = 0;

    };

}

#endif
