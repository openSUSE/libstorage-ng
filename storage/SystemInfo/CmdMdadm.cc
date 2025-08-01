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


#include <locale>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdMdadm.h"
#include "storage/Devices/MdImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{
    using namespace std;


    CmdMdadmDetail::CmdMdadmDetail(const string& device)
	: device(device)
    {
	SystemCmd cmd({ MDADM_BIN, "--detail", device, "--export" }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdMdadmDetail::parse(const vector<string>& lines)
    {
	for (const string& line1 : lines)
	{
	    string::size_type pos1 = line1.find('=');
	    if (pos1 == string::npos)
		continue;

	    string key1 = line1.substr(0, pos1);
	    string value1 = string(line1, pos1 + 1);

	    if (key1 == "MD_UUID")
	    {
		uuid = value1;
	    }
	    else if (key1 == "MD_DEVNAME")
	    {
		devname = value1;
	    }
	    else if (key1 == "MD_METADATA")
	    {
		metadata = value1;
	    }
	    else if (key1 == "MD_LEVEL")
	    {
		level = toValueWithFallback(boost::to_upper_copy(value1, locale::classic()),
					    MdLevel::UNKNOWN);
		if (level == MdLevel::UNKNOWN)
		    y2war("unknown raid type " << value1);
	    }
	    else if (boost::starts_with(key1, "MD_DEVICE_") && boost::ends_with(key1, "_DEV"))
	    {
		// Check if the line is a MD_DEVICE_xxx_DEV line and
		// if it is find the corresponding MD_DEVICE_xxx_ROLE
		// line and save the role.

		string key2 = string(key1, 0, pos1 - strlen("DEV")) + "ROLE";

		for (const string& line2 : lines)
		{
		    if (boost::starts_with(line2, key2 + "="))
		    {
			string value2 = string(line2, key2.size() + 1);
			roles[value1] = value2;
			break;
		    }
		}
	    }
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdMdadmDetail& cmd_mdadm_detail)
    {
	s << "device:" << cmd_mdadm_detail.device << " uuid:" << cmd_mdadm_detail.uuid << " devname:"
	  << cmd_mdadm_detail.devname << " metadata:" << cmd_mdadm_detail.metadata << " level:"
	  << toString(cmd_mdadm_detail.level) << " roles:" << cmd_mdadm_detail.roles << '\n';

	return s;
    }

}
