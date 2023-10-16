/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2017-2023] SUSE LLC
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


#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/SystemInfo/CmdDasdview.h"
#include "storage/Devices/DasdImpl.h"


namespace storage
{

    Dasdview::Dasdview(const string& device)
	: device(device), bus_id(), type(DasdType::UNKNOWN), format(DasdFormat::NONE)
    {
	SystemCmd cmd({ DASDVIEW_BIN, "--extended", device }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    Dasdview::parse(const vector<string>& lines)
    {
	vector<string>::const_iterator pos;

	pos = find_if(lines, string_starts_with("busid"));
	if (pos != lines.end())
	{
	    y2mil("Bus-ID line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    bus_id = extractNthWord(0, tmp);
	}

	pos = find_if(lines, string_starts_with("format"));
	if (pos != lines.end())
	{
	    y2mil("Format line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(4, tmp);
	    if (tmp == "CDL")
		format = DasdFormat::CDL;
	    else if (tmp == "LDL")
		format = DasdFormat::LDL;
	}

	pos = find_if(lines, string_starts_with("type"));
	if (pos != lines.end())
	{
	    y2mil("Type line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(0, tmp);
	    if (tmp == "ECKD")
		type = DasdType::ECKD;
	    else if (tmp == "FBA")
		type = DasdType::FBA;
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const Dasdview& dasdview)
    {
	s << "device:" << dasdview.device << " bus-id:" << dasdview.bus_id << " type:"
	  << toString(dasdview.type) << " format:" << toString(dasdview.format);

	return s;
    }

}
