/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#include "storage/SystemInfo/CmdDasdview.h"
#include "storage/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Enum.h"
#include "storage/StorageTypes.h"


namespace storage_bgl
{

    Dasdview::Dasdview(const string& device, bool do_probe)
	: device(device), dasd_format(DASDF_NONE), dasd_type(DASDTYPE_NONE)
    {
	if (do_probe)
	    probe();
    }


    void
    Dasdview::probe()
    {
	SystemCmd cmd(DASDVIEWBIN " --extended " + quote(device));

	if (cmd.retcode() == 0)
	{
	    parse(cmd.stdout());
	}
	else
	{
	    y2err("dasdview failed");

	    geometry.heads = 15;
	    geometry.sectors = 12;
	    geometry.sector_size = 4096;
	}
    }


    void
    Dasdview::parse(const vector<string>& lines)
    {
	vector<string>::const_iterator pos;

	pos = find_if(lines, string_starts_with("format"));
	if (pos != lines.end())
	{
	    y2mil("Format line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(4, tmp);
	    if (tmp == "CDL")
		dasd_format = DASDF_CDL;
	    else if (tmp == "LDL")
		dasd_format = DASDF_LDL;
	}

	pos = find_if(lines, string_starts_with("type"));
	if (pos != lines.end())
	{
	    y2mil("Type line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(0, tmp);
	    if (tmp == "ECKD")
		dasd_type = DASDTYPE_ECKD;
	    else if (tmp == "FBA")
		dasd_type = DASDTYPE_FBA;
	}

	pos = find_if(lines, string_starts_with("number of cylinders"));
	if (pos != lines.end())
	{
	    y2mil("Cylinder line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(3, tmp);
	    tmp >> geometry.cylinders;
	}

	pos = find_if(lines, string_starts_with("tracks per cylinder"));
	if (pos != lines.end())
	{
	    y2mil("Tracks line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(3, tmp);
	    tmp >> geometry.heads;
	}

	pos = find_if(lines, string_starts_with("blocks per track"));
	if (pos != lines.end())
	{
	    y2mil("Blocks line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(3, tmp);
	    tmp >> geometry.sectors;
	}

	pos = find_if(lines, string_starts_with("blocksize"));
	if (pos != lines.end())
	{
	    y2mil("Bytes line:" << *pos);
	    string tmp = string(*pos, pos->find(':') + 1);
	    tmp = extractNthWord(3, tmp);
	    tmp >> geometry.sector_size;
	}

	y2mil(*this);
    }


    std::ostream& operator<<(std::ostream& s, const Dasdview& dasdview)
    {
	s << "device:" << dasdview.device << " geometry:" << dasdview.geometry
	  << " dasd_format:" << toString(dasdview.dasd_format) << " dasd_type:"
	  << toString(dasdview.dasd_type);

	return s;
    }

}
