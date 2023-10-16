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


#include "storage/SystemInfo/CmdDumpe2fs.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageTypes.h"


namespace storage
{

    CmdDumpe2fs::CmdDumpe2fs(const string& device)
	: device(device)
    {
	SystemCmd cmd({ DUMPE2FS_BIN, "-h", device }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdDumpe2fs::parse(const vector<string>& lines)
    {
	vector<string>::const_iterator pos;

	pos = find_if(lines, string_starts_with("Block size:"));
	if (pos != lines.end())
	{
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp >> block_size;
	}
	else
	{
	    ST_THROW(Exception("failed to find block size line in dumpe2fs output"));
	}

	pos = find_if(lines, string_starts_with("Filesystem features:"));
	if (pos != lines.end())
	{
	    string tmp = string(*pos, pos->find(':') + 1);
	    vector<string> features;
	    boost::split(features, tmp, boost::is_any_of("\t "), boost::token_compress_on);

	    feature_64bit = contains(features, "64bit");
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDumpe2fs& cmd_dumpe2fs)
    {
	return s << "device:" << cmd_dumpe2fs.device << " block-size:" << cmd_dumpe2fs.block_size
		 << " feature-64bit:" << cmd_dumpe2fs.feature_64bit;
    }

}
