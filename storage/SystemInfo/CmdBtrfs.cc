/*
 * Copyright (c) [2004-2015] Novell, Inc.
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

#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/SystemInfo/CmdBtrfs.h"


namespace storage
{
    using namespace std;


    CmdBtrfsShow::CmdBtrfsShow()
    {
	SystemCmd cmd( BTRFSBIN " filesystem show", SystemCmd::DoThrow );

	if ( cmd.retcode() == 0 && !cmd.stdout().empty() )
	    parse( cmd.stdout() );
	else if ( ! cmd.stderr().empty() )
	{
	    ST_THROW( SystemCmdException( &cmd, "'btrfs filesystem show' complains: "
					  + cmd.stderr().front() ) );
	}

	// Intentionally not throwing an exception here if retcode != 0 since
	// this command might return 1 if no btrfs at all was found -- which is
	// not an error condition: We are probing here to determine if there
	// are any btrfs file systems, and if yes, some more information about
	// them.

	// stdout is rarely empty for this command since in almost all cases it
	// at least reports its version number, so this is also not very useful
	// to indicate errors.
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
		Entry entry;
		++it;
		while( it!=lines.end() && !boost::contains( *it, " uuid: " ) &&
		       !boost::contains( *it, "devid " ) )
		    ++it;

		while( it!=lines.end() && boost::contains( *it, "devid " ) )
		{
		    y2mil( "devs line:" << *it );
		    string device = extractNthWord( 7, *it );
		    if ( !boost::contains( device, "/dev" ) )  // Allow /sys/dev or /proc/devices
			ST_THROW( ParseException( "Not a valid device name", device, "/dev/..." ) );
		    entry.devices.push_back( device );
		    ++it;
		}

		if ( entry.devices.empty() )
		{
		    ST_THROW( ParseException( "No devices for UUID " + uuid, "",
					      "devid  1 size 40.00GiB used 16.32GiB path /dev/sda2" ) );
		}

		y2mil( "devs:" << entry.devices );
		data[ uuid ] = entry;
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


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsShow& cmdbtrfsshow)
    {
	for (CmdBtrfsShow::const_iterator it = cmdbtrfsshow.data.begin(); it != cmdbtrfsshow.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << endl;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsShow::Entry& entry)
    {
	s << entry.devices;

	return s;
    }


    CmdBtrfsSubvolumes::CmdBtrfsSubvolumes(const string& mount_point)
    {
	SystemCmd cmd(BTRFSBIN " subvolume list -a -p " + quote(mount_point), SystemCmd::DoThrow);

	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
	else
	    ST_THROW(SystemCmdException(&cmd, "'btrfs subvolume list' failed, ret: " +
					to_string(cmd.retcode())));
    }


    void
    CmdBtrfsSubvolumes::parse(const vector<string>& lines)
    {
	for (const string& line : lines)
	{
	    Entry entry;

	    string parent;
	    string::size_type pos1 = line.find(" parent ");
	    if (pos1 != string::npos)
		pos1 = line.find_first_not_of(app_ws, pos1 + 6);
	    if (pos1 != string::npos)
		parent = line.substr(pos1, line.find_last_not_of(app_ws));

	    // Subvolume can already be deleted, in which case parent is "0"
	    // (and path "DELETED"). That is a temporary state.
	    if (parent == "0")
		continue;

	    string::size_type pos2 = line.find(" path ");
	    if (pos2 != string::npos)
		pos2 = line.find_first_not_of(app_ws, pos2 + 5);
	    if (pos2 != string::npos)
		entry.path = line.substr(pos2, line.find_last_not_of(app_ws));
	    if (boost::starts_with(entry.path, "<FS_TREE>/"))
		entry.path.erase(0, 10);

	    data.push_back(entry);
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsSubvolumes& cmdbtrfssubvolumes)
    {
	for (const CmdBtrfsSubvolumes::Entry& entry : cmdbtrfssubvolumes)
	    s << "data " << entry.path << endl;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsSubvolumes::Entry& entry)
    {
	s << entry.path;

	return s;
    }

}
