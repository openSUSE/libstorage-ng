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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/StorageDefines.h"
#include "storage/SystemInfo/CmdDmraid.h"


namespace storage_bgl
{
    using namespace std;


    CmdDmraid::CmdDmraid(bool do_probe)
    {
	if (do_probe)
	    probe();
    }


    void
    CmdDmraid::probe()
    {
	SystemCmd c(DMRAIDBIN " -s -c -c -c");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdDmraid::parse(const vector<string>& lines)
    {
	data.clear();

	vector<string>::const_iterator it = lines.begin();
	while (it != lines.end())
	{
	    const list<string> sl = splitString(*it++, ":");
	    if (sl.size() >= 4)
	    {
		Entry entry;

		list<string>::const_iterator ci = sl.begin();
		string name = *ci;
		advance(ci, 3);
		entry.raidtype = *ci;

		while (it != lines.end() && boost::starts_with(*it, "/dev/"))
		{
		    const list<string> sl = splitString(*it, ":");
		    if (sl.size() >= 4)
		    {
			list<string>::const_iterator ci = sl.begin();
			entry.devices.push_back(*ci);
			advance(ci, 1);
			entry.controller = *ci;
		    }

		    ++it;
		}

		data[name] = entry;
	    }
	}

	for (const_iterator it = data.begin(); it != data.end(); ++it)
	    y2mil("data[" << it->first << "] -> controller:" << it->second.controller <<
		  " raidtype:" << it->second.raidtype << " devices:" << it->second.devices);
    }


    list<string>
    CmdDmraid::getEntries() const
    {
	list<string> ret;
	for (const_iterator it = data.begin(); it != data.end(); ++it)
	    ret.push_back(it->first);
	return ret;
    }


    bool
    CmdDmraid::getEntry(const string& name, Entry& entry) const
    {
	const_iterator it = data.find(name);
	if (it == data.end())
	    return false;

	entry = it->second;
	return true;
    }


    std::ostream& operator<<(std::ostream& s, const CmdDmraid& cmddmraid)
    {
	for (CmdDmraid::const_iterator it = cmddmraid.begin(); it != cmddmraid.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const CmdDmraid::Entry& entry)
    {
	s << "raidtype:" << entry.raidtype << " controller:" << entry.controller
	  << " devices:" << entry.devices;

	return s;
    }

}
