/*
 * Copyright (c) [2017-2025] SUSE LLC
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


#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/SystemInfo/CmdDf.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{
    using namespace std;


    CmdDf::CmdDf(const string& path)
	: path(path)
    {
	SystemCmd cmd({ DF_BIN, "--block-size=1", "--output=size,used,avail,fstype", path });
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    void
    CmdDf::parse(const vector<string>& lines)
    {
	if (lines.size() != 2)
	    ST_THROW(Exception("parse error"));

	std::istringstream data(lines[1]);
	classic(data);

	data >> size >> used >> available;

	if (size < used + available)
	    y2war("implausible df values");

	string tmp;
	data >> tmp;
	if (!toValue(tmp, fs_type))
	    y2war("unknown fs-type '" << tmp << "'");

	if (data.fail())
	    ST_THROW(Exception("parse error"));

	y2mil(*this);
    }


    SpaceInfo
    CmdDf::get_space_info() const
    {
	// At least btrfs can have available = 0 but still have used < size. That should
	// show up as full (100%) anyway.

	return SpaceInfo(used + available, used);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDf& cmd_df)
    {
	s << "path:" << cmd_df.path << " size:" << cmd_df.size << " used:" << cmd_df.used
	  << " available:" << cmd_df.available << " fs-type:" << toString(cmd_df.fs_type) << '\n';

	return s;
    }

}
