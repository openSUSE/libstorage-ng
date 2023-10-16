/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdNtfsresize.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    CmdNtfsresize::CmdNtfsresize(const string& device)
	: device(device), min_size(0)
    {
	SystemCmd cmd({ NTFSRESIZE_BIN, "--force", "--info", device }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdNtfsresize::parse(const vector<string>& lines)
    {
	static const string fstr = " might resize at ";

	string stdout = boost::join(lines, "\n");

	string::size_type pos1 = stdout.find(fstr);
	if (pos1 == string::npos)
	    ST_THROW(Exception("failed to parse output of ntfsresize"));
	y2mil("pos1:" << pos1);

	string::size_type pos2 = stdout.find_first_not_of(" \t\n", pos1 + fstr.size());
	y2mil("pos2:" << pos2);

	string number = stdout.substr(pos2, stdout.find_first_not_of("0123456789", pos2));
	number >> min_size;

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdNtfsresize& cmd_ntfsresize)
    {
	return s << "device:" << cmd_ntfsresize.device << " min-size:" << cmd_ntfsresize.min_size;
    }

}
