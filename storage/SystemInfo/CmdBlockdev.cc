/*
 * Copyright (c) [2021-2023] SUSE LLC
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
#include "storage/SystemInfo/CmdBlockdev.h"


namespace storage
{
    using namespace std;


    CmdBlockdev::CmdBlockdev(const string& path)
	: path(path)
    {
	SystemCmd cmd({ BLOCKDEV_BIN, "--getsize64", path });

	if (cmd.retcode() == 0 && cmd.stdout().size() >= 1)
	    parse(cmd.stdout());

	y2mil(*this);
    }


    void
    CmdBlockdev::parse(const vector<string>& lines)
    {
	size = stoull(lines[0]);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBlockdev& cmd_blockdev)
    {
	s << "path:" << cmd_blockdev.path << " size:" << cmd_blockdev.size << '\n';

        return s;
    }

}
