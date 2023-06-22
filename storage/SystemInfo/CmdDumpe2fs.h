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


#ifndef STORAGE_CMD_DUMPE2FS_H
#define STORAGE_CMD_DUMPE2FS_H


#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;


    /**
     * Class to run the command "dumpe2fs", parse the output and save
     * some values.
     */
    class CmdDumpe2fs
    {

    public:

	CmdDumpe2fs(const string& device);

	friend std::ostream& operator<<(std::ostream& s, const CmdDumpe2fs& cmd_dumpe2fs);

	unsigned long get_block_size() const { return block_size; }
	bool has_feature_64bit() const { return feature_64bit; }

    private:

	void parse(const vector<string>& lines);

	const string device;

	unsigned long block_size = 0;
	bool feature_64bit = false;

    };

}

#endif
