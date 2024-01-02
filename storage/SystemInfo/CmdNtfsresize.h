/*
 * Copyright (c) 2015 Novell, Inc.
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


#ifndef STORAGE_CMD_NTFSRESIZE_H
#define STORAGE_CMD_NTFSRESIZE_H


#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;


    /**
     * Class to run the command "ntfsresize", parse the output and
     * save some values.
     */
    class CmdNtfsresize
    {

    public:

	CmdNtfsresize(const string& device);

	friend std::ostream& operator<<(std::ostream& s, const CmdNtfsresize& cmd_ntfsresize);

	unsigned long long get_min_size() const { return min_size; }

    private:

	void parse(const vector<string>& lines);

	const string device;

	unsigned long long min_size = 0;

    };

}

#endif
