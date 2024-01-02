/*
 * Copyright (c) [2019-2023] SUSE LLC
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


#include "storage/SystemInfo/CmdResize2fs.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    CmdResize2fs::CmdResize2fs(const string& device)
	: device(device)
    {
	SystemCmd cmd({ RESIZE2FS_BIN, "-P", device }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdResize2fs::parse(const vector<string>& lines)
    {
	vector<string>::const_iterator pos =
	    find_if(lines, string_starts_with("Estimated minimum size of the filesystem:"));
	if (pos != lines.end())
	{
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp >> min_blocks;
	}
	else
	{
	    ST_THROW(Exception("failed to find min size line in resize2fs output"));
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdResize2fs& cmd_resize2fs)
    {
	return s << "device:" << cmd_resize2fs.device << " min-blocks:" << cmd_resize2fs.min_blocks;
    }

}
