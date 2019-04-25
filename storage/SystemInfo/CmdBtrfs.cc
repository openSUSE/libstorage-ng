/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2017-2019] SUSE LLC
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


#include <locale>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/SystemInfo/CmdBtrfs.h"
#include "storage/Filesystems/BtrfsImpl.h"


namespace storage
{
    using namespace std;


    CmdBtrfsFilesystemShow::CmdBtrfsFilesystemShow()
    {
	SystemCmd::Options cmd_options(BTRFSBIN " filesystem show");
	cmd_options.throw_behaviour = SystemCmd::DoThrow;
	cmd_options.verify = [](int) { return true; };

	SystemCmd cmd(cmd_options);

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
    CmdBtrfsFilesystemShow::parse(const vector<string>& lines)
    {
	static const regex uuid_regex("uuid: (" UUID_REGEX ")", regex::extended);

	smatch match;

	vector<string>::const_iterator it = lines.begin();

	while (it != lines.end())
	{
	    while( it != lines.end() && !boost::contains( *it, " uuid: " ))
		++it;

	    if( it!=lines.end() )
	    {
		y2mil( "uuid line:" << *it );

		Entry entry;

		if (!regex_search(*it, match, uuid_regex))
		    ST_THROW(Exception("did not find uuid"));

		entry.uuid = match[1];
		y2mil("uuid:" << entry.uuid);

		++it;
		while( it!=lines.end() && !boost::contains( *it, " uuid: " ) &&
		       !boost::contains( *it, "devid " ) )
		    ++it;

		while( it!=lines.end() && boost::contains( *it, "devid " ) )
		{
		    y2mil( "devs line:" << *it );

		    Device device;

		    extractNthWord(1, *it) >> device.id;

		    device.name = extractNthWord(7, *it);
		    if (!boost::contains(device.name, DEV_DIR "/"))  // Allow /sys/dev or /proc/devices
			ST_THROW( ParseException( "Not a valid device name", device.name, "/dev/..." ) );

		    entry.devices.push_back( device );
		    ++it;
		}

		if ( entry.devices.empty() )
		{
		    ST_THROW( ParseException( "No devices for UUID " + entry.uuid, "",
					      "devid  1 size 40.00GiB used 16.32GiB path /dev/sda2" ) );
		}

		y2mil("devices:" << entry.devices);

		data.push_back(entry);
	    }
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsFilesystemShow& cmd_btrfs_filesystem_show)
    {
	for (const CmdBtrfsFilesystemShow::Entry& entry : cmd_btrfs_filesystem_show)
	    s << entry << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsFilesystemShow::Entry& entry)
    {
	return s << "uuid:" << entry.uuid << " devices:" << entry.devices;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsFilesystemShow::Device& device)
    {
	return s << "{ id:" << device.id << " name:" << device.name << " }";
    }


    CmdBtrfsSubvolumeList::CmdBtrfsSubvolumeList(const key_t& key, const string& mount_point)
    {
	SystemCmd::Options cmd_options(BTRFSBIN " subvolume list -a -p " + quote(mount_point));
	cmd_options.mockup_key = BTRFSBIN " subvolume list -a -p (device:" + key + ")";
	cmd_options.throw_behaviour = SystemCmd::DoThrow;

	SystemCmd cmd(cmd_options);
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
	else
	    ST_THROW(SystemCmdException(&cmd, "'btrfs subvolume list' failed, ret: " +
					to_string(cmd.retcode())));
    }


    void
    CmdBtrfsSubvolumeList::parse(const vector<string>& lines)
    {
	for (const string& line : lines)
	{
	    Entry entry;

	    string::size_type pos1 = line.find("ID ");
	    line.substr(pos1 + 3) >> entry.id;

	    string::size_type pos2 = line.find(" parent ");
	    line.substr(pos2 + 8) >> entry.parent_id;

	    // Subvolume can already be deleted, in which case parent is "0"
	    // (and path "DELETED"). That is a temporary state.
	    if (entry.parent_id == 0)
		continue;

	    string::size_type pos3 = line.find(" path ");
	    entry.path = line.substr(pos3 + 6);
	    if (boost::starts_with(entry.path, "<FS_TREE>/"))
		entry.path.erase(0, 10);

	    data.push_back(entry);
	}

	y2mil(*this);
    }


    CmdBtrfsSubvolumeList::const_iterator
    CmdBtrfsSubvolumeList::find_entry_by_path(const string& path) const
    {
	for (const_iterator it = data.begin(); it != data.end(); ++it)
	{
	    if (it->path == path)
		return it;
	}

	return data.end();
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsSubvolumeList& cmd_btrfs_subvolume_list)
    {
	for (const CmdBtrfsSubvolumeList::Entry& entry : cmd_btrfs_subvolume_list)
	    s << entry;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsSubvolumeList::Entry& entry)
    {
	s << "id:" << entry.id << " parent-id:" << entry.parent_id
	  << " path:" << entry.path << '\n';

	return s;
    }


    CmdBtrfsSubvolumeGetDefault::CmdBtrfsSubvolumeGetDefault(const key_t& key, const string& mount_point)
	: id(BtrfsSubvolume::Impl::unknown_id)
    {
	SystemCmd::Options cmd_options(BTRFSBIN " subvolume get-default " + quote(mount_point));
	cmd_options.mockup_key = BTRFSBIN " subvolume get-default (device:" + key + ")";
	cmd_options.throw_behaviour = SystemCmd::DoThrow;

	SystemCmd cmd(cmd_options);
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
	else
	    ST_THROW(SystemCmdException(&cmd, "'btrfs subvolume get-default' failed, ret: " +
					to_string(cmd.retcode())));
    }


    void
    CmdBtrfsSubvolumeGetDefault::parse(const vector<string>& lines)
    {
	if (lines.size() != 1)
	    ST_THROW(Exception("output has wrong number of lines"));

	const string& line = lines[0];

	if (!boost::starts_with(line, "ID "))
	    ST_THROW(Exception("output does not start with ID"));

	line.substr(3) >> id;

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsSubvolumeGetDefault& cmd_btrfs_subvolume_get_default)
    {
	s << "id:" << cmd_btrfs_subvolume_get_default.id;

	return s;
    }


    CmdBtrfsFilesystemDf::CmdBtrfsFilesystemDf(const key_t& key, const string& mount_point)
	: metadata_raid_level(BtrfsRaidLevel::UNKNOWN), data_raid_level(BtrfsRaidLevel::UNKNOWN)
    {
	SystemCmd::Options cmd_options(BTRFSBIN " filesystem df " + quote(mount_point));
	cmd_options.mockup_key = BTRFSBIN " filesystem df (device:" + key + ")";
	cmd_options.throw_behaviour = SystemCmd::DoThrow;

	SystemCmd cmd(cmd_options);
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
	else
	    ST_THROW(SystemCmdException(&cmd, "'btrfs filesystem df' failed, ret: " +
					to_string(cmd.retcode())));
    }


    void
    CmdBtrfsFilesystemDf::parse(const vector<string>& lines)
    {
	static const regex metadata_rx("Metadata, ([A-Za-z0-9]+):.*", regex::extended);
	static const regex data_rx("Data, ([A-Za-z0-9]+):.*", regex::extended);
	static const regex mixed_rx("Data\\+Metadata, ([A-Za-z0-9]+):.*", regex::extended);

	smatch match;

	for (const string& line : lines)
	{
	    if (regex_match(line, match, metadata_rx) && match.size() == 2)
	    {
		string tmp = boost::to_upper_copy(match[1].str(), locale::classic());
		metadata_raid_level = toValueWithFallback(tmp, BtrfsRaidLevel::UNKNOWN);
	    }

	    if (regex_match(line, match, data_rx) && match.size() == 2)
	    {
		string tmp = boost::to_upper_copy(match[1].str(), locale::classic());
		data_raid_level = toValueWithFallback(tmp, BtrfsRaidLevel::UNKNOWN);
	    }

	    if (regex_match(line, match, mixed_rx) && match.size() == 2)
	    {
		string tmp = boost::to_upper_copy(match[1].str(), locale::classic());
		metadata_raid_level = data_raid_level = toValueWithFallback(tmp, BtrfsRaidLevel::UNKNOWN);
	    }
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsFilesystemDf& cmd_btrfs_filesystem_df)
    {
	s << "metadata-raid-level:" << toString(cmd_btrfs_filesystem_df.metadata_raid_level)
	  << " data-raid-level:" << toString(cmd_btrfs_filesystem_df.data_raid_level);

	return s;
    }

}
