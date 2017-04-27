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


#include <sys/sysmacros.h>

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
    CmdDmsetupInfo::exists(const string& name) const
    {
	return data.find(name) != data.end();
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmsetupInfo& cmd_dmsetup_info)
    {
	for (const CmdDmsetupInfo::value_type& it : cmd_dmsetup_info)
	    s << "data[" << it.first << "] -> " << it.second << '\n';

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

	if (lines.size() == 1 && lines[0] == "No devices found")
	    return;

	for (const string& line : lines)
	{
	    string::size_type pos = line.find(": ");
	    if (pos == string::npos)
		ST_THROW(Exception("failed to parse dmsetup table output"));

	    string name = line.substr(0, pos);

	    list<string> tmp = splitString(line.substr(pos + 1));
	    vector<string> params = vector<string>(tmp.begin(), tmp.end());

	    if (params.size() < 3)
		ST_THROW(Exception("failed to parse dmsetup table output"));

	    Table table(params[2]);

	    if (table.target == "striped")
	    {
		params[3] >> table.stripes;
		params[4] >> table.stripe_size;
		table.stripe_size *= 512;
	    }

	    for (const string& param : params)
	    {
		if (devspec.match(param))
		{
		    unsigned int major, minor;

		    devspec.cap(1) >> major;
		    devspec.cap(2) >> minor;

		    dev_t majorminor = makedev(major, minor);
		    table.majorminors.push_back(majorminor);
		}
	    }

	    data[name].push_back(table);
	}

	y2mil(*this);
    }


    vector<CmdDmsetupTable::Table>
    CmdDmsetupTable::get_tables(const string& name) const
    {
	const_iterator it = data.find(name);
	if (it == data.end())
	    ST_THROW(Exception("dm table not found"));

	return it->second;
    }


    CmdDmsetupTable::const_iterator
    CmdDmsetupTable::find_using(dev_t majorminor) const
    {
	for (const_iterator it = begin(); it != end(); ++it)
	{
	    for (const Table& table : it->second)
	    {
		if (contains(table.majorminors, majorminor))
		    return it;
	    }
	}

	return data.end();
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmsetupTable& cmd_dmsetup_table)
    {
	for (const CmdDmsetupTable::value_type& it : cmd_dmsetup_table)
	    for (const CmdDmsetupTable::Table& table : it.second)
		s << "data[" << it.first << "] -> " << table << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmsetupTable::Table& table)
    {
	s << "target:" << table.target;

	if (table.stripes != 0)
	    s << " stripes:" << table.stripes;

	if (table.stripe_size != 0)
	    s << " stripe-size:" << table.stripe_size;

	s << " majorminors:<";
	for (vector<dev_t>::const_iterator it = table.majorminors.begin(); it != table.majorminors.end(); ++it)
	{
	    if (it != table.majorminors.begin())
		s << " ";
	    s << gnu_dev_major(*it) << ":" << gnu_dev_minor(*it);
	}
	s << ">";

	return s;
    }

}
