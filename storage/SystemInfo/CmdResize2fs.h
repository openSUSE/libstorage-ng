/*
 * Copyright (c) 2019 SUSE LLC
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


#ifndef STORAGE_CMD_RESIZE2FS_H
#define STORAGE_CMD_RESIZE2FS_H


#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;


    /**
     * Class to run the command "resize2fs", parse the output and save
     * some values.
     */
    class CmdResize2fs
    {

    public:

	CmdResize2fs(const string& device);

	friend std::ostream& operator<<(std::ostream& s, const CmdResize2fs& cmd_resize2fs);

	unsigned long long get_min_blocks() const { return min_blocks; }

    private:

	void parse(const vector<string>& lines);

	string device;

	unsigned long long min_blocks;

    };

}

#endif
