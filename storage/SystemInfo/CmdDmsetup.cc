/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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
#include <regex>

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
	SystemCmd cmd({ DMSETUP_BIN, "--columns", "--separator", "/", "--noheadings", "-o", "name,major,minor,"
		"segments,subsystem,uuid", "info" }, SystemCmd::DoThrow);

	parse(cmd.stdout());
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

		unsigned int major, minor;
		*ci++ >> major;
		*ci++ >> minor;
		entry.majorminor = makedev(major, minor);

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
	s << "major:" << major(entry.majorminor) << " minor:" << minor(entry.majorminor)
	  << " segments:" << entry.segments << " subsystem:" << entry.subsystem
	  << " uuid:" << entry.uuid;

	return s;
    }


    CmdDmsetupTable::CmdDmsetupTable()
    {
	SystemCmd cmd({ DMSETUP_BIN, "table" }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdDmsetupTable::parse(const vector<string>& lines)
    {
	static const regex devspec("([0-9]+):([0-9]+)", regex::extended);

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
		smatch match;

		if (regex_match(param, match, devspec) && match.size() == 3)
		{
		    unsigned int major, minor;
		    match[1] >> major;
		    match[2] >> minor;
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
	    s << major(*it) << ":" << minor(*it);
	}
	s << ">";

	return s;
    }

}
