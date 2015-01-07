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
#include "storage/Utils/AsciiFile.h"
#include "storage/SystemInfo/ProcMounts.h"
#include "storage/Utils/StorageTmpl.h"


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


    std::ostream& operator<<(std::ostream& s, const ProcMounts& procmounts)
    {
	for (ProcMounts::const_iterator it = procmounts.data.begin(); it != procmounts.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << endl;

	return s;
    }


    string
    ProcMounts::getMount(const string& device) const
    {
	string ret;
	const_iterator i = data.find(device);
	if (i != data.end())
	    ret = i->second.mount;
	return ret;
    }


    string
    ProcMounts::getMount(const list<string>& devices) const
    {
	string ret;
	list<string>::const_iterator i = devices.begin();
	while (ret.empty() && i != devices.end())
	{
	    ret = getMount( *i );
	    ++i;
	}
	return ret;
    }


    list<string>
    ProcMounts::getAllMounts(const string& device) const
    {
	list<string> ret;

	pair<const_iterator, const_iterator> range = data.equal_range(device);
	for (const_iterator i = range.first; i != range.second; ++i)
	    ret.push_back(i->second.mount);

	return ret;
    }


    list<string>
    ProcMounts::getAllMounts(const list<string>& devices) const
    {
	list<string> ret;

	for (list<string>::const_iterator i = devices.begin(); i != devices.end(); ++i)
	    ret.splice(ret.end(), getAllMounts(*i));

	return ret;
    }


    map<string, string>
    ProcMounts::allMounts() const
    {
	map<string, string> ret;

	for (const_iterator i = data.begin(); i != data.end(); ++i)
	    ret[i->second.mount] = i->first;

	return ret;
    }


    list<FstabEntry>
    ProcMounts::getEntries() const
    {
	list<FstabEntry> ret;

	for (const_iterator i = data.begin(); i != data.end(); ++i)
	    ret.push_back(i->second);

	return ret;
    }

}
