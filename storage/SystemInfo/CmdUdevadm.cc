/*
 * Copyright (c) 2015 Novell, Inc.
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


#include <sys/sysmacros.h>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/CmdUdevadm.h"


namespace storage
{
    using namespace std;


    CmdUdevadmInfo::CmdUdevadmInfo(const string& file)
	: file(file), path(), name(), majorminor(0), by_path_links(), by_id_links()
    {
	// Without emptying the udev queue 'udevadm info' can display old data
	// or even complain about unknown devices. Even during probing this
	// can happen since e.g. 'parted' opens the disk device read-write
	// even when all parted commands are read-only, thus triggering udev
	// events. So always run 'udevadm settle'.
	SystemCmd(UDEVADMBIN_SETTLE);

	SystemCmd cmd(UDEVADMBIN " info " + quote(file), SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdUdevadmInfo::parse(const vector<string>& stdout)
    {
	unsigned int major = 0;
	unsigned int minor = 0;

	for (const string& line : stdout)
	{
	    if (boost::starts_with(line, "P: "))
		line.substr(2) >> path;

	    if (boost::starts_with(line, "N: "))
		line.substr(2) >> name;

	    if (boost::starts_with(line, "E: MAJOR="))
		line.substr(9) >> major;

	    if (boost::starts_with(line, "E: MINOR="))
		line.substr(9) >> minor;

	    if (boost::starts_with(line, "S: disk/by-path/"))
		by_path_links.push_back(line.substr(16));

	    if (boost::starts_with(line, "S: disk/by-id/"))
		by_id_links.push_back(line.substr(14));
	}

	majorminor = makedev(major, minor);

	sort(by_path_links.begin(), by_path_links.end());
	sort(by_id_links.begin(), by_id_links.end());

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdUdevadmInfo& cmdudevadminfo)
    {
	s << "file:" << cmdudevadminfo.file << " path:" << cmdudevadminfo.get_path()
	  << " name:" << cmdudevadminfo.get_name() << " majorminor:"
	  << cmdudevadminfo.get_major() << ":" << cmdudevadminfo.get_minor();

	if (!cmdudevadminfo.by_path_links.empty())
	    s << " by-path-links:" << cmdudevadminfo.by_path_links;

	if (!cmdudevadminfo.by_id_links.empty())
	    s << " by-id-links:" << cmdudevadminfo.by_id_links;

	s << '\n';

	return s;
    }

}
