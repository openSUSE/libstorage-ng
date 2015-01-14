/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/DevAndSys.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{
    using namespace std;


    MajorMinor::MajorMinor(const string& device)
	: device(device)
    {
	SystemCmd cmd(STATBIN " --dereference --format '%F 0x%t:0x%T' " + quote(device));
	if (cmd.retcode() != 0)
	    throw runtime_error("stat failure for " + device);

	parse(cmd.stdout());

	y2mil(*this);
    }


    void
    MajorMinor::parse(const vector<string>& lines)
    {
	if (lines.size() != 1)
	    throw runtime_error("parse error");

	unsigned int major = 0, minor = 0;

	if (sscanf(lines[0].c_str(), "block special file %X:%X", &major, &minor) != 2)
	    throw runtime_error("parse error");

	majorminor = makedev(major, minor);
    }


    std::ostream&
    operator<<(std::ostream& s, const MajorMinor& majorminor)
    {
	s << "device:" << majorminor.device << " majorminor:" << majorminor.getMajor()
	  << ":" << majorminor.getMinor();

	return s;
    }


    Dir::Dir(const string& path)
	: path(path)
    {
	SystemCmd cmd(LSBIN " -1 --sort=none " + quote(path));
	if (cmd.retcode() != 0)
	    throw runtime_error("ls failure for " + path);

	parse(cmd.stdout());

	y2mil(*this);
    }


    void
    Dir::parse(const vector<string>& lines)
    {
	entries = list<string>(lines.begin(), lines.end());
    }


    std::ostream& operator<<(std::ostream& s, const Dir& dir)
    {
	s << "path:" << dir.path << " entries:" << dir.entries << endl;

	return s;
    }


    map<string, string>
    DevLinks::getDirLinks(const string& path) const
    {
	// TODO use cmd(STATBIN " --format '%F %N' " + quote(path) + "/*")?

	SystemCmd cmd(LSBIN " -1l --sort=none " + quote(path));
	if (cmd.retcode() != 0)
	    throw runtime_error("ls failure for " + path);

	return parse(cmd.stdout());
    }


    map<string, string>
    DevLinks::parse(const vector<string>& lines) const
    {
	map<string, string> ret;

	for (const string& line : lines)
	{
	    list<string> tmp = splitString(line);
	    if (tmp.size() >= 3)
	    {
		string v = tmp.back();
		tmp.pop_back();

		if (tmp.back() == "->")
		{
		    tmp.pop_back();

		    string k = tmp.back();

		    ret[k] = v;
		}
	    }
	}

	return ret;
    }


    std::ostream& operator<<(std::ostream& s, const DevLinks& devlinks)
    {
	for (const DevLinks::value_type& it : devlinks)
	    s << "data[" << it.first << "] -> " << boost::join(it.second, " ") << endl;

	return s;
    }


    MdLinks::MdLinks()
    {
	map<string, string> links = getDirLinks("/dev/md");
	for (const map<string, string>::value_type& it : links)
	{
	    string::size_type pos = it.second.find_first_not_of("./");
	    if (pos != string::npos)
	    {
		data[it.second.substr(pos)].push_back(it.first);
	    }
	}

	y2mil(*this);
    }

}
