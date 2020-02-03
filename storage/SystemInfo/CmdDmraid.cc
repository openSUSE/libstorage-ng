/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) 2017 SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/Mockup.h"
#include "storage/SystemInfo/CmdDmraid.h"


namespace storage
{
    using namespace std;


    CmdDmraid::CmdDmraid()
    {
	SystemCmd cmd(DMRAID_BIN " --sets=active -ccc");
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    void
    CmdDmraid::parse(const vector<string>& lines)
    {
	data.clear();

	LinesIterator lines_iterator(lines);

	while (!lines_iterator.at_end())
	    scan_superset_line(lines_iterator);

	y2mil(*this);
    }


    void
    CmdDmraid::scan_superset_line(LinesIterator& lines_iterator)
    {
	vector<string> tmp;
	boost::split(tmp, lines_iterator.pop_line(), boost::is_any_of(":"));

	if (tmp.size() < 8)
	    ST_THROW(Exception("missing fields"));

	Entry entry;

	entry.raid_type = tmp[3];

	int subsets = 0;
	tmp[5] >> subsets;

	for (int subset = 0; subset < subsets; ++subset)
	    scan_subset_line(lines_iterator, entry);

	int devices = 0;
	tmp[6] >> devices;

	devices -= entry.devices.size();

	for (int device = 0; device < devices; ++device)
	    scan_device_line(lines_iterator, entry);

	data[tmp[0]] = entry;
    }


    void
    CmdDmraid::scan_subset_line(LinesIterator& lines_iterator, Entry& entry)
    {
	vector<string> tmp;
	boost::split(tmp, lines_iterator.pop_line(), boost::is_any_of(":"));

	if (tmp.size() < 8)
	    ST_THROW(Exception("missing fields"));

	int devices = 0;
	tmp[6] >> devices;

	for (int device = 0; device < devices; ++device)
	    scan_device_line(lines_iterator, entry);
    }


    void
    CmdDmraid::scan_device_line(LinesIterator& lines_iterator, Entry& entry)
    {
	vector<string> tmp;
	boost::split(tmp, lines_iterator.pop_line(), boost::is_any_of(":"));

	if (tmp.size() < 7)
	    ST_THROW(Exception("missing fields"));

	entry.devices.push_back(tmp[0]);

	entry.controller = tmp[1];
    }


    vector<string>
    CmdDmraid::get_entries() const
    {
	vector<string> ret;
	for (const_iterator it = data.begin(); it != data.end(); ++it)
	    ret.push_back(it->first);
	return ret;
    }


    const CmdDmraid::Entry&
    CmdDmraid::get_entry(const string& name) const
    {
	const_iterator it = data.find(name);
	if (it == data.end())
	    ST_THROW(Exception("entry not found"));

	return it->second;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmraid& cmd_dmraid)
    {
	for (const pair<string, CmdDmraid::Entry>& entry : cmd_dmraid)
	    s << "data[" << entry.first << "] -> " << entry.second << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdDmraid::Entry& entry)
    {
	s << "raid-type:" << entry.raid_type << " controller:" << entry.controller
	  << " devices:" << entry.devices;

	return s;
    }

}
