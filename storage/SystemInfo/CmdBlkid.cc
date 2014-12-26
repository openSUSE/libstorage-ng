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


#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdBlkid.h"
#include "storage/Utils/Enum.h"


namespace storage
{
    using namespace std;


    Blkid::Blkid(bool do_probe)
    {
	if (do_probe)
	    probe();
    }


    void
    Blkid::probe()
    {
	SystemCmd cmd("BLKID_SKIP_CHECK_MDRAID=1 " BLKIDBIN " -c /dev/null");
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    Blkid::Blkid(const string& device)
    {
	SystemCmd cmd("BLKID_SKIP_CHECK_MDRAID=1 " BLKIDBIN " -c /dev/null " + quote(device));
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    void
    Blkid::parse(const vector<string>& lines)
    {
	static const map<string, FsType> fs_table = {
	    { "btrfs", BTRFS },
	    { "ext2", EXT2 },
	    { "ext3", EXT3 },
	    { "ext4", EXT4 },
	    { "hfs", HFS },
	    { "hfsplus", HFSPLUS },
	    { "jfs", JFS },
	    { "msdos", VFAT },
	    { "ntfs", NTFS },
	    { "ntfs-3g", NTFS },
	    { "reiserfs", REISERFS },
	    { "swap", SWAP },
	    { "vfat", VFAT },
	    { "xfs", XFS },
	    { "iso9660", ISO9660 },
	    { "udf", UDF }
	};

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

	    map<string, string>::const_iterator i = m.find("TYPE");
	    if (i != m.end())
	    {
		map<string, FsType>::const_iterator it2 = fs_table.find(i->second);
		if (it2 != fs_table.end())
		{
		    entry.is_fs = true;
		    entry.fs_type = it2->second;
		}
		else if (i->second == "LVM2_member")
		{
		    entry.is_lvm = true;
		}
		else if (i->second == "crypto_LUKS")
		{
		    entry.is_luks = true;
		}
	    }

	    if (entry.is_fs)
	    {
		i = m.find("UUID");
		if (i != m.end())
		    entry.fs_uuid = i->second;

		i = m.find("LABEL");
		if (i != m.end())
		    entry.fs_label = i->second;
	    }

	    if (entry.is_luks)
	    {
		i = m.find("UUID");
		if (i != m.end())
		    entry.luks_uuid = i->second;
	    }

	    if (entry.is_fs || entry.is_lvm || entry.is_luks)
		data[device] = entry;
	}

	y2mil(*this);
    }


    bool
    Blkid::getEntry(const string& device, Entry& entry) const
    {
	const_iterator i = data.find(device);
	if (i == data.end())
	    return false;

	entry = i->second;
	return true;
    }


    std::ostream& operator<<(std::ostream& s, const Blkid& blkid)
    {
	for (Blkid::const_iterator it = blkid.data.begin(); it != blkid.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const Blkid::Entry& entry)
    {
	if (entry.is_fs)
	{
	    s << "is_fs:" << entry.is_fs;
	    s << " fs_type:" << toString(entry.fs_type);
	    if (!entry.fs_uuid.empty())
		s << " fs_uuid:" << entry.fs_uuid;
	    if (!entry.fs_label.empty())
		s << " fs_label:" << entry.fs_label;
	}

	if (entry.is_lvm)
	{
	    s << "is_lvm:" << entry.is_lvm;
	}

	if (entry.is_luks)
	{
	    s << "is_luks:" << entry.is_luks;
	    if (!entry.luks_uuid.empty())
		s << " luks_uuid:" << entry.luks_uuid;
	}

	return s;
    }

}
