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


#include "storage/Utils/Regex.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/CmdDmsetup.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{
    using namespace std;


    CmdDmsetupInfo::CmdDmsetupInfo()
    {
	SystemCmd c(DMSETUPBIN " --columns --separator '/' --noheadings -o name,major,minor,"
		    "segments,subsystem,uuid info");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdDmsetupInfo::parse(const vector<string>& lines)
    {
	for (const string& line : lines)
	{
	    list<string> sl = splitString(line, "/");
	    if (sl.size() >= 6)
	    {
		Entry entry;

		list<string>::const_iterator ci = sl.begin();
		string name = *ci++;
		*ci++ >> entry.mjr;
		*ci++ >> entry.mnr;
		*ci++ >> entry.segments;
		*ci++ >> entry.subsystem;
		entry.uuid = *ci++;

		data[name] = entry;
	    }
	}

	y2mil(*this);
    }


    bool
    CmdDmsetupInfo::getEntry(const string& name, Entry& entry) const
    {
	const_iterator it = data.find(name);
	if (it == data.end())
	    return false;

	entry = it->second;
	return true;
    }


    vector<string>
    CmdDmsetupInfo::getEntries() const
    {
	vector<string> ret;
	for (const value_type& value : data)
	    ret.push_back(value.first);
	return ret;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmsetupInfo& cmd_dmsetup_info)
    {
	for (const CmdDmsetupInfo::value_type& it : cmd_dmsetup_info)
	    s << "data[" << it.first << "] -> " << it.second << endl;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmsetupInfo::Entry& entry)
    {
	s << "mjr:" << entry.mjr << " mnr:" << entry.mnr << " segments:" << entry.segments
	  << " subsystem:" << entry.subsystem << " uuid:" << entry.uuid;

	return s;
    }


    CmdDmsetupTable::CmdDmsetupTable()
    {
	SystemCmd c(DMSETUPBIN " table");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdDmsetupTable::parse(const vector<string>& lines)
    {
	static Regex devspec("^([0123456789]+):([0123456789]+)$");

	for (const string& line : lines)
	{
	    string::size_type pos = line.find(": ");
	    if (pos == string::npos)
		ST_THROW(Exception("failed to parse dmsetup table output"));

	    string name = line.substr(0, pos);

	    Entry& entry = data[name];

	    list<string> params = splitString(line.substr(pos));
	    for (const string& param : params)
	    {
		if (devspec.match(param))
		{
		    unsigned int major, minor;

		    devspec.cap(1) >> major;
		    devspec.cap(2) >> minor;

		    dev_t majorminor = makedev(major, minor);
		    entry.majorminors.push_back(majorminor);
		}
	    }
	}

	y2mil(*this);
    }


    bool
    CmdDmsetupTable::getEntry(const string& name, Entry& entry) const
    {
	const_iterator it = data.find(name);
	if (it == data.end())
	    return false;

	entry = it->second;
	return true;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmsetupTable& cmd_dmsetup_table)
    {
	for (const CmdDmsetupTable::value_type& it : cmd_dmsetup_table)
	    s << "data[" << it.first << "] -> " << it.second << endl;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmsetupTable::Entry& entry)
    {
	s << "majorminors:<";
	for (vector<dev_t>::const_iterator it = entry.majorminors.begin(); it != entry.majorminors.end(); ++it)
	{
	    if (it != entry.majorminors.begin())
		s << " ";
	    s << gnu_dev_major(*it) << ":" << gnu_dev_minor(*it);
	}
	s << ">";

	return s;
    }

}
