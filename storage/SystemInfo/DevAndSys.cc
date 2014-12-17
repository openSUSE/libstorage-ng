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


#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/DevAndSys.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{
    using namespace std;


    MajorMinor::MajorMinor(const string& device, bool do_probe)
	: device(device)
    {
	if (do_probe)
	    probe();
    }


    void
    MajorMinor::probe()
    {
	struct stat buf;
	if (stat(device.c_str(), &buf) != 0)
	    throw runtime_error(device + " not found");

	if (!S_ISBLK(buf.st_mode))
	    throw runtime_error(device + " not block device");

	majorminor = buf.st_rdev;

	y2mil(*this);
    }


    std::ostream& operator<<(std::ostream& s, const MajorMinor& majorminor)
    {
	s << "device:" << majorminor.device << " majorminor:" << majorminor.getMajor()
	  << ":" << majorminor.getMinor();

	return s;
    }


    Dir::Dir(const string& path, bool do_probe)
	: path(path)
    {
	if (do_probe)
	    probe();
    }


    void
    Dir::probe()
    {
	entries = getDir(path);

	y2mil(*this);
    }


    std::ostream& operator<<(std::ostream& s, const Dir& dir)
    {
	s << "path:" << dir.path << " entries:" << dir.entries << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const DevLinks& devlinks)
    {
	for (const DevLinks::value_type& it : devlinks)
	    s << "data[" << it.first << "] -> " << boost::join(it.second, " ") << endl;

	return s;
    }


    UdevMap::UdevMap(const string& path, bool do_probe)
	: path(path)
    {
	if (do_probe)
	    probe();
    }


    void
    UdevMap::probe()
    {
	map<string, string> links = getDirLinks(path);
	for (const map<string, string>::value_type& it : links)
	{
	    string::size_type pos = it.second.find_first_not_of("./");
	    if (pos != string::npos)
	    {
		string tmp = it.second.substr(pos);
		if (boost::starts_with(tmp, "dev/"))
		    tmp.erase(0, 4);
		data[tmp].push_back(udevDecode(it.first));
	    }
	}

	y2mil(*this);
    }


    std::ostream& operator<<(std::ostream& s, const UdevMap& udevmap)
    {
	s << "path:" << udevmap.path << endl;
	s << dynamic_cast<const DevLinks&>(udevmap);

	return s;
    }


    MdLinks::MdLinks(bool do_probe)
    {
	if (do_probe)
	    probe();
    }


    void
    MdLinks::probe()
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
