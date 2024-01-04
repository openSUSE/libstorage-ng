/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2018-2023] SUSE LLC
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


    CmdUdevadmInfo::CmdUdevadmInfo(Udevadm& udevadm, const string& file)
	: file(file)
    {
	// Without emptying the udev queue 'udevadm info' can display old data
	// or even complain about unknown devices. Even during probing this
	// can happen since e.g. 'parted' opens the disk device read-write
	// even when all parted commands are read-only, thus triggering udev
	// events (fixed in SUSE versions). So always run 'udevadm settle'.
	udevadm.settle();

	SystemCmd::Options options({ UDEVADM_BIN, "info", file }, SystemCmd::DoThrow);
	options.env.push_back("SYSTEMD_COLORS=false");
	SystemCmd cmd(options);

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
	    { "S: disk/by-label/", by_label_links },
	    { "S: disk/by-uuid/", by_uuid_links },
	    { "S: disk/by-partlabel/", by_partlabel_links },
	    { "S: disk/by-partuuid/", by_partuuid_links },
	    { "S: mapper/", mapper_links },
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

	    for (const Link& link : links)
	    {
		if (boost::starts_with(line, link.name))
		{
		    link.variable.push_back(line.substr(strlen(link.name)));
		    break;
		}
	    }
	}

	if (path.empty())
	    ST_THROW(Exception("no path information found in udevadm info output"));

	if (name.empty())
	    ST_THROW(Exception("no name information found in udevadm info output"));

	majorminor = makedev(major, minor);

	for (const Link& link : links)
	    sort(link.variable.begin(), link.variable.end());

	y2mil(*this);
    }


    bool
    CmdUdevadmInfo::is_alias_of(const string& file) const
    {
	struct Link
	{
	    const char* prefix;
	    const vector<string>& variable;
	};

	const Link links[] = {
	    { DEV_DISK_BY_PATH_DIR "/", by_path_links },
	    { DEV_DISK_BY_ID_DIR "/", by_id_links },
	    { DEV_DISK_BY_LABEL_DIR "/", by_label_links },
	    { DEV_DISK_BY_UUID_DIR "/", by_uuid_links },
	    { DEV_DISK_BY_PARTLABEL_DIR "/", by_partlabel_links },
	    { DEV_DISK_BY_PARTUUID_DIR "/", by_partuuid_links },
	    { DEV_MAPPER_DIR "/", mapper_links },
	};

	for (const Link& link : links)
	    for (const string& tmp : link.variable)
		if (link.prefix + tmp == file)
		    return true;

	return false;
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

	if (!cmd_udevadm_info.by_label_links.empty())
	    s << " by-label-links:" << cmd_udevadm_info.by_label_links;

	if (!cmd_udevadm_info.by_uuid_links.empty())
	    s << " by-uuid-links:" << cmd_udevadm_info.by_uuid_links;

	if (!cmd_udevadm_info.by_partlabel_links.empty())
	    s << " by-partlabel-links:" << cmd_udevadm_info.by_partlabel_links;

	if (!cmd_udevadm_info.by_partuuid_links.empty())
	    s << " by-partuuid-links:" << cmd_udevadm_info.by_partuuid_links;

	if (!cmd_udevadm_info.mapper_links.empty())
	    s << " mapper-links:" << cmd_udevadm_info.mapper_links;

	s << '\n';

	return s;
    }

}
