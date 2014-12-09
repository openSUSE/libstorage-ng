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


#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/CmdDmsetup.h"
#include "storage/Utils/AppUtil.h"
#include "storage/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage_bgl
{
    using namespace std;


    CmdDmsetupInfo::CmdDmsetupInfo(bool do_probe)
    {
	if (do_probe)
	    probe();
    }


    void
    CmdDmsetupInfo::probe()
    {
	SystemCmd c(DMSETUPBIN " --columns --separator '/' --noheadings -o name,major,minor,segments,uuid info");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdDmsetupInfo::parse(const vector<string>& lines)
    {
	for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
	    list<string> sl = splitString(*it, "/");
	    if (sl.size() >= 5)
	    {
		Entry entry;

		list<string>::const_iterator ci = sl.begin();
		string name = *ci++;
		*ci++ >> entry.mjr;
		*ci++ >> entry.mnr;
		*ci++ >> entry.segments;
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


    list<string>
    CmdDmsetupInfo::getEntries() const
    {
	list<string> ret;
	for (const_iterator i = data.begin(); i != data.end(); ++i)
	    ret.push_back(i->first);
	return ret;
    }


    std::ostream& operator<<(std::ostream& s, const CmdDmsetupInfo& cmddmsetupinfo)
    {
	for (const CmdDmsetupInfo::value_type& it : cmddmsetupinfo)
	    s << "data[" << it.first << "] -> " << it.second << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const CmdDmsetupInfo::Entry& entry)
    {
	s << "mjr:" << entry.mjr << " mnr:" << entry.mnr << " segments:" << entry.segments
	  << " uuid:" << entry.uuid;

	return s;
    }

}
