/*
 * Copyright (c) [2018-2023] SUSE LLC
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


#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdStat.h"


namespace storage
{
    using namespace std;


    CmdStat::CmdStat(const string& path)
	: path(path)
    {
	SystemCmd cmd({ STAT_BIN, "--format", "%f", path });

	if (cmd.retcode() == 0 && cmd.stdout().size() >= 1)
	    parse(cmd.stdout());

	y2mil(*this);
    }


    void
    CmdStat::parse(const vector<string>& lines)
    {
	mode = stoi(lines[0], nullptr, 16);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdStat& cmd_stat)
    {
	s << "path:" << cmd_stat.path << " mode:" << cmd_stat.mode << '\n';

        return s;
    }

}
