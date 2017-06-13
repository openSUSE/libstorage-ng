/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#include <algorithm>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/SystemInfo/CmdBlkid.h"
#include "storage/Filesystems/FilesystemImpl.h"


namespace storage
{
    using namespace std;


    Blkid::Blkid()
    {
	SystemCmd cmd(BLKIDBIN " -c '/dev/null'");
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    Blkid::Blkid(const string& device)
    {
	SystemCmd cmd(BLKIDBIN " -c '/dev/null' " + quote(device));
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    void
    Blkid::parse(const vector<string>& lines)
    {
	data.clear();

	for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
	    string::size_type pos = it->find(":");
	    if (pos == string::npos)
		continue;

	    string device = string(*it, 0, pos);
	    list<string> l = splitString(string(*it, pos + 1), " \t\n", true, true, "\"");

	    Entry entry;

	    const map<string, string> m = makeMap(l, "=", "\"");

	    map<string, string>::const_iterator it1 = m.find("TYPE");
	    if (it1 != m.end())
	    {
		if (toValue(it1->second, entry.fs_type, false))
		{
		    entry.is_fs = true;
		}
		else if (it1->second == "linux_raid_member")
		{
		    entry.is_md = true;
		}
		else if (it1->second == "LVM2_member")
		{
		    entry.is_lvm = true;
		}
		else if (it1->second == "crypto_LUKS")
		{
		    entry.is_luks = true;
		}
		else if (it1->second == "bcache")
		{
		    entry.is_bcache = true;
		}
	    }

	    if (entry.is_fs)
	    {
		it1 = m.find("UUID");
		if (it1 != m.end())
		    entry.fs_uuid = it1->second;

		it1 = m.find("LABEL");
		if (it1 != m.end())
		    entry.fs_label = it1->second;
	    }

	    if (entry.is_luks)
	    {
		it1 = m.find("UUID");
		if (it1 != m.end())
		    entry.luks_uuid = it1->second;
	    }

	    if (entry.is_bcache)
	    {
		it1 = m.find("UUID");
		if (it1 != m.end())
		    entry.bcache_uuid = it1->second;
	    }

	    if (entry.is_fs || entry.is_md || entry.is_lvm || entry.is_luks || entry.is_bcache)
		data[device] = entry;
	}

	y2mil(*this);
    }


    bool
    Blkid::find_by_name(const string& device, Entry& entry, SystemInfo& systeminfo) const
    {
	const_iterator it = data.find(device);
	if (it != data.end())
	{
	    entry = it->second;
	    return true;
	}

	dev_t majorminor = systeminfo.getCmdUdevadmInfo(device).get_majorminor();

	for (const value_type& value : data)
	{
	    if (systeminfo.getCmdUdevadmInfo(value.first).get_majorminor() == majorminor)
	    {
		entry = value.second;
		return true;
	    }
	}

	return false;
    }


    bool
    Blkid::get_sole_entry(Entry& entry) const
    {
	if (data.size() == 1)
	{
	    entry = data.begin()->second;
	    return true;
	}

	return false;
    }


    bool
    Blkid::any_md() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_md; });
    }


    bool
    Blkid::any_lvm() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_lvm; });
    }


    bool
    Blkid::any_luks() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_luks; });
    }


    bool
    Blkid::any_bcache() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_bcache; });
    }


    std::ostream&
    operator<<(std::ostream& s, const Blkid& blkid)
    {
	for (Blkid::const_iterator it = blkid.data.begin(); it != blkid.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const Blkid::Entry& entry)
    {
	if (entry.is_fs)
	{
	    s << "is-fs:" << entry.is_fs;
	    s << " fs-type:" << toString(entry.fs_type);
	    if (!entry.fs_uuid.empty())
		s << " fs-uuid:" << entry.fs_uuid;
	    if (!entry.fs_label.empty())
		s << " fs-label:" << entry.fs_label;
	}

	if (entry.is_md)
	{
	    s << "is-md:" << entry.is_md;
	}

	if (entry.is_lvm)
	{
	    s << "is-lvm:" << entry.is_lvm;
	}

	if (entry.is_luks)
	{
	    s << "is-luks:" << entry.is_luks;
	    if (!entry.luks_uuid.empty())
		s << " luks-uuid:" << entry.luks_uuid;
	}

	if (entry.is_bcache)
	{
	    s << "is-bcache:" << entry.is_bcache;
	    if (!entry.bcache_uuid.empty())
		s << " bcache-uuid:" << entry.bcache_uuid;
	}

	return s;
    }

}
