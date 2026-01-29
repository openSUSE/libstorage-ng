/*
 * Copyright (c) [2015-2017] SUSE LLC
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
#include "storage/SystemInfo/CmdLsattr.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/LoggerImpl.h"


namespace storage
{
    using namespace std;


    CmdLsattr::CmdLsattr(const key_t& key, const string& mount_point, const string& path)
	: mount_point(mount_point), path(path)
    {
	SystemCmd::Options cmd_options({ LSATTR_BIN, "-d", mount_point + "/" + path }, SystemCmd::DoThrow);
	cmd_options.mockup_key = LSATTR_BIN " -d (device:" + get<0>(key) + " path:" + get<1>(key) + ")";

	SystemCmd cmd(cmd_options);

	parse(cmd.stdout());
    }


    void
    CmdLsattr::parse(const vector<string>& lines)
    {
	if (lines.size() != 1)
	    ST_THROW(ParseException("wrong number of lines", to_string(lines.size()), to_string(1)));

	const string& line = lines.front();

	string::size_type pos = line.find(' ');
	if (pos == string::npos)
	    ST_THROW(ParseException("no space found", line, "---------------C /var/lib/mariadb"));

	string attrs = line.substr(0, pos);

	nocow = attrs.find('C') != string::npos;

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLsattr& cmd_lsattr)
    {
	s << "mount-point:" << cmd_lsattr.mount_point << " path:" << cmd_lsattr.path;

	if (cmd_lsattr.nocow)
	    s << " nocow:" << cmd_lsattr.nocow;

	return s;
    }

}
