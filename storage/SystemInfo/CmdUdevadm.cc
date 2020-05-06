/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2018-2020] SUSE LLC
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


    const vector<string> EnumTraits<CmdUdevadmInfo::DeviceType>::names({
	"unknown", "disk", "partition"
    });


    CmdUdevadmInfo::CmdUdevadmInfo(const string& file)
	: file(file), path(), name(), majorminor(0), device_type(DeviceType::UNKNOWN),
	  by_path_links(), by_id_links(), by_part_label_links(), by_part_uuid_links()
    {
	// Without emptying the udev queue 'udevadm info' can display old data
	// or even complain about unknown devices. Even during probing this
	// can happen since e.g. 'parted' opens the disk device read-write
	// even when all parted commands are read-only, thus triggering udev
	// events (fixed in recent versions). So always run 'udevadm settle'.
	SystemCmd(UDEVADM_BIN_SETTLE);

	SystemCmd cmd(UDEVADM_BIN " info " + quote(file), SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdUdevadmInfo::parse(const vector<string>& stdout)
    {
	unsigned int major = 0;
	unsigned int minor = 0;

	struct Link
	{
	    const char* name;
	    vector<string>& variable;
	};

	const Link links[] = {
	    { "S: disk/by-path/", by_path_links },
	    { "S: disk/by-id/", by_id_links },
	    { "S: disk/by-partlabel/", by_part_label_links },
	    { "S: disk/by-partuuid/", by_part_uuid_links }
	};

	for (const string& line : stdout)
	{
	    if (boost::starts_with(line, "P: "))
		path = line.substr(strlen("P: "));

	    if (boost::starts_with(line, "N: "))
		name = line.substr(strlen("N: "));

	    if (boost::starts_with(line, "E: MAJOR="))
		line.substr(strlen("E: MAJOR=")) >> major;

	    if (boost::starts_with(line, "E: MINOR="))
		line.substr(strlen("E: MINOR=")) >> minor;

	    if (boost::starts_with(line, "E: DEVTYPE="))
		device_type = toValueWithFallback(line.substr(strlen("E: DEVTYPE=")), DeviceType::UNKNOWN);

	    for (Link link : links)
		if (boost::starts_with(line, link.name))
		    link.variable.push_back(line.substr(strlen(link.name)));
	}

	if (path.empty())
	    ST_THROW(Exception("no path information found in udevadm info output"));

	if (name.empty())
	    ST_THROW(Exception("no name information found in udevadm info output"));

	majorminor = makedev(major, minor);

	for (Link link : links)
	    sort(link.variable.begin(), link.variable.end());

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdUdevadmInfo& cmd_udevadm_info)
    {
	s << "file:" << cmd_udevadm_info.file << " path:" << cmd_udevadm_info.get_path()
	  << " name:" << cmd_udevadm_info.get_name() << " majorminor:"
	  << cmd_udevadm_info.get_major() << ":" << cmd_udevadm_info.get_minor()
	  << " device-type:" << toString(cmd_udevadm_info.get_device_type());

	if (!cmd_udevadm_info.by_path_links.empty())
	    s << " by-path-links:" << cmd_udevadm_info.by_path_links;

	if (!cmd_udevadm_info.by_id_links.empty())
	    s << " by-id-links:" << cmd_udevadm_info.by_id_links;

	if (!cmd_udevadm_info.by_part_label_links.empty())
	    s << " by-part-label-links:" << cmd_udevadm_info.by_part_label_links;

	if (!cmd_udevadm_info.by_part_uuid_links.empty())
	    s << " by-part-uuid-links:" << cmd_udevadm_info.by_part_uuid_links;

	s << '\n';

	return s;
    }

}
