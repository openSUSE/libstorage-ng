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

#include "storage/StorageTypes.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/CmdBtrfs.h"
#include "storage/Utils/AppUtil.h"
#include "storage/StorageDefines.h"


namespace storage
{
    using namespace std;


    CmdBtrfsShow::CmdBtrfsShow(bool do_probe)
    {
	if (do_probe)
	    probe();
    }


    void
    CmdBtrfsShow::probe()
    {
	SystemCmd c(BTRFSBIN " filesystem show");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdBtrfsShow::parse(const vector<string>& lines)
    {
	vector<string>::const_iterator it = lines.begin();

	while (it != lines.end())
	{
	    while( it != lines.end() && !boost::contains( *it, " uuid: " ))
		++it;

	    if( it!=lines.end() )
	    {
		y2mil( "uuid line:" << *it );
		string uuid = extractNthWord( 3, *it );
		y2mil( "uuid:" << uuid );
		Entry e;
		++it;
		while( it!=lines.end() && !boost::contains( *it, " uuid: " ) &&
		       !boost::contains( *it, "devid " ) )
		    ++it;
		while( it!=lines.end() && boost::contains( *it, "devid " ) )
		{
		    y2mil( "devs line:" << *it );
		    e.devices.push_back( extractNthWord( 7, *it ));
		    ++it;
		}
		y2mil( "devs:" << e.devices );
		data[uuid] = e;
	    }
	}

	y2mil(*this);
    }


    bool
    CmdBtrfsShow::getEntry(const string& uuid, Entry& entry) const
    {
	const_iterator it = data.find(uuid);
	if (it!=data.end())
	    entry = it->second;
	return it != data.end();
    }


    list<string>
    CmdBtrfsShow::getUuids() const
    {
	list<string> ret;

	for (const_iterator it = data.begin(); it != data.end(); ++it)
	    ret.push_back(it->first);

	y2mil("ret:" << ret);
	return ret;
    }


    std::ostream& operator<<(std::ostream& s, const CmdBtrfsShow& cmdbtrfsshow)
    {
	for (CmdBtrfsShow::const_iterator it = cmdbtrfsshow.data.begin(); it != cmdbtrfsshow.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const CmdBtrfsShow::Entry& entry)
    {
	s << entry.devices;

	return s;
    }

}
