/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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


#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{
    using namespace std;


    ProcMounts::ProcMounts()
    {
	AsciiFile mounts("/proc/mounts");
	AsciiFile swaps("/proc/swaps");

	parse(mounts.lines(), swaps.lines());
    }


    void
    ProcMounts::parse(const vector<string>& mount_lines, const vector<string>& swap_lines)
    {
	data.clear();

	for (vector<string>::const_iterator it = mount_lines.begin(); it != mount_lines.end(); ++it)
	{
	    string dev = boost::replace_all_copy(extractNthWord(0, *it), "\\040", " ");
	    string dir = boost::replace_all_copy(extractNthWord(1, *it), "\\040", " ");

	    if (dev == "rootfs" || dev == "/dev/root")
	    {
		y2mil("skipping line:" << *it);
		continue;
	    }

	    FstabEntry entry;
	    entry.device = dev;
	    entry.mount = dir;
	    entry.fs = extractNthWord(2, *it);
	    entry.opts = splitString(extractNthWord(3, *it), ",");
	    data.insert(make_pair(dev, entry));
	}

	for (vector<string>::const_iterator it = swap_lines.begin(); it != swap_lines.end(); ++it)
	{
	    if (it == swap_lines.begin())
		continue;

	    string dev = boost::replace_all_copy(extractNthWord(0, *it), "\\040", " ");
	    string::size_type pos = dev.find(" (deleted)");
	    if (pos != string::npos)
		dev.erase(pos);

	    FstabEntry entry;
	    entry.device = dev;
	    entry.mount = "swap";
	    entry.fs = "swap";
	    data.insert(make_pair(dev, entry));
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const ProcMounts& procmounts)
    {
	for (ProcMounts::const_iterator it = procmounts.data.begin(); it != procmounts.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << '\n';

	return s;
    }


    vector<string>
    ProcMounts::find_by_name(const string& name, SystemInfo& systeminfo) const
    {
	vector<string> ret;

	// TODO: Lookup with major and minor number only for names of block
	// devices (starting with '/dev/'). Parameter name will also be
	// e.g. 'tmpfs' and nfs mounts.

	dev_t majorminor = systeminfo.getCmdUdevadmInfo(name).get_majorminor();

	for (const value_type& value : data)
	{
	    if (value.first == name ||
		(BlkDevice::Impl::is_valid_name(value.first) &&
		 systeminfo.getCmdUdevadmInfo(value.first).get_majorminor() == majorminor))
		ret.push_back(value.second.mount);
	}

	return ret;
    }

}
