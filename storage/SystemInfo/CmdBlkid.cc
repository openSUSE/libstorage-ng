/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2026] SUSE LLC
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


#include <cctype>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdBlkid.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/EnvironmentImpl.h"


namespace storage
{
    using namespace std;


    Blkid::Blkid(Udevadm& udevadm)
    {
	const bool json = CmdBlkidVersion::supports_json_option_v2();

	udevadm.settle();

	SystemCmd::Options options({ BLKID_BIN, "--cache-file", DEV_NULL_FILE }, SystemCmd::DoThrow);

	// If blkid does not find anything it returns 2 (see bsc #1203285).
	options.verify = [](int exit_code) { return exit_code == 0 || exit_code == 2; };

	SystemCmd cmd(options);

	parse(cmd.stdout());
    }


    Blkid::Blkid(Udevadm& udevadm, const string& device)
    {
	const bool json = CmdBlkidVersion::supports_json_option_v2();

	udevadm.settle();

	SystemCmd::Options options({ BLKID_BIN, "--cache-file", DEV_NULL_FILE, device }, SystemCmd::DoThrow);
	options.verify = [](int exit_code) { return exit_code == 0 || exit_code == 2; };

	SystemCmd cmd(options);

	parse(cmd.stdout());
    }


    void
    Blkid::parse(const vector<string>& lines)
    {
	data.clear();

	for (const string& line : lines)
	{
	    string::size_type pos = line.find(": ");
	    if (pos == string::npos)
		continue;

	    string device = string(line, 0, pos);
	    list<string> l = split_line(string(line, pos + 1));

	    Entry entry;

	    const map<string, string> m = makeMap(l, "=", "\"");

	    map<string, string>::const_iterator it1 = m.find("TYPE");
	    if (it1 != m.end())
	    {
		if (it1->second == "BitLocker" && cryptsetup_for_bitlocker())
		{
		    entry.is_bitlocker = true;
		}
		else if (toValue(it1->second, entry.fs_type, false))
		{
		    entry.is_fs = true;
		}
		else if (it1->second == "jbd" || it1->second == "xfs_external_log")
		{
		    entry.is_journal = true;
		}
		else if (boost::ends_with(it1->second, "_raid_member"))
		{
		    entry.is_md = true;
		}
		else if (it1->second == "LVM2_member")
		{
		    entry.is_lvm = true;
		}
		else if (it1->second == "crypto_LUKS")
		{
		    entry.is_luks = true;
		}
		else if (it1->second == "bcache")
		{
		    entry.is_bcache = true;
		}
	    }

	    if (entry.is_fs)
	    {
		it1 = m.find("UUID");
		if (it1 != m.end())
		    entry.fs_uuid = it1->second;

		it1 = m.find("LABEL");
		if (it1 != m.end())
		    entry.fs_label = it1->second;

		it1 = m.find("EXT_JOURNAL");
		if (it1 != m.end())
		    entry.fs_journal_uuid = it1->second;

		it1 = m.find("UUID_SUB");
		if (it1 != m.end())
		    entry.fs_sub_uuid = it1->second;
	    }

	    if (entry.is_journal)
	    {
		it1 = m.find("LOGUUID");
		if (it1 != m.end())
		    entry.journal_uuid = it1->second;
	    }

	    if (entry.is_luks)
	    {
		it1 = m.find("UUID");
		if (it1 != m.end())
		    entry.luks_uuid = it1->second;

		it1 = m.find("LABEL");
		if (it1 != m.end())
		    entry.luks_label = it1->second;
	    }

	    if (entry.is_bitlocker)
	    {
		// Unfortunately no UUID although BitLocker has one.
	    }

	    if (entry.is_bcache)
	    {
		it1 = m.find("UUID");
		if (it1 != m.end())
		    entry.bcache_uuid = it1->second;
	    }

	    if (entry.is_fs || entry.is_journal || entry.is_md || entry.is_lvm || entry.is_luks ||
		entry.is_bitlocker || entry.is_bcache)
		data[device] = entry;
	}

	y2mil(*this);
    }


    list<string>
    Blkid::split_line( const string & line )
    {
        list<string> result;

        string::size_type start = 0;
        const string::size_type len = line.length();

        while ( start < len )
        {
            // Skip leading whitespace
            while ( isspace( line[start] ) && start < len - 1 )
                ++start;

            string::size_type end = start;

            while ( end < len )
            {
                if ( isspace( line[end] ) )
                    break;

                if ( line[end] == '"' ) // found a quoted block
                {
                    ++end;              // skip opening quote

                    // Find the end of the quoted block
                    while ( end < len - 1 )
                    {
                        if ( line[++end] == '"' &&      // quote
                             line[end - 1] != '\\' )    // but not an escaped quote
                            break;

                        // Notice that line[end - 1] is safe:
                        //
                        // If we are at the start of this segment, it will point to
                        // the opening quote. If there would be no opening quote,
                        // we wouldn't be in this branch at all.
                    }
                }

                ++end;
            }

            // Add the segment we just found to the result
            if ( start < end && start < len )
                result.push_back( line.substr( start, end - start ) );

            // Start over with the next segment
            start = end + 1;
        }

        return result;
    }


    Blkid::const_iterator
    Blkid::find_by_any_name(const string& device, SystemInfo::Impl& system_info) const
    {
	const_iterator it = data.find(device);
	if (it != end())
	    return it;

	dev_t majorminor = system_info.getCmdUdevadmInfo(device).get_majorminor();
	return find_if(begin(), end(), [&system_info, &majorminor](const value_type& tmp) {
	    return system_info.getCmdUdevadmInfo(tmp.first).get_majorminor() == majorminor;
	});
    }


    Blkid::const_iterator
    Blkid::find_by_journal_uuid(const string& journal_uuid) const
    {
	return find_if(begin(), end(), [&journal_uuid](const value_type& tmp) {
	    return tmp.second.is_journal && tmp.second.journal_uuid == journal_uuid;
	});
    }


    Blkid::const_iterator
    Blkid::get_sole_entry() const
    {
	return data.size() == 1 ? begin() : end();
    }


    bool
    Blkid::any_md() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_md; });
    }


    bool
    Blkid::any_lvm() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_lvm; });
    }


    bool
    Blkid::any_luks() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_luks; });
    }


    bool
    Blkid::any_bitlocker() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_bitlocker; });
    }


    bool
    Blkid::any_bcache() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) { return value.second.is_bcache; });
    }


    bool
    Blkid::any_btrfs() const
    {
	return std::any_of(data.begin(), data.end(), [](const value_type& value) {
	    return value.second.is_fs && value.second.fs_type == FsType::BTRFS;
	});
    }


    std::ostream&
    operator<<(std::ostream& s, const Blkid& blkid)
    {
	for (Blkid::const_iterator it = blkid.data.begin(); it != blkid.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const Blkid::Entry& entry)
    {
	if (entry.is_fs)
	{
	    s << "is-fs:" << entry.is_fs;
	    s << " fs-type:" << toString(entry.fs_type);
	    if (!entry.fs_uuid.empty())
		s << " fs-uuid:" << entry.fs_uuid;
	    if (!entry.fs_label.empty())
		s << " fs-label:" << entry.fs_label;
	    if (!entry.fs_journal_uuid.empty())
		s << " fs-journal-uuid:" << entry.fs_journal_uuid;
	    if (!entry.fs_sub_uuid.empty())
		s << " fs-sub-uuid:" << entry.fs_sub_uuid;
	}

	if (entry.is_journal)
	{
	    s << "is-journal:" << entry.is_journal;
	    if (!entry.journal_uuid.empty())
		s << " journal-uuid:" << entry.journal_uuid;
	}

	if (entry.is_md)
	{
	    s << "is-md:" << entry.is_md;
	}

	if (entry.is_lvm)
	{
	    s << "is-lvm:" << entry.is_lvm;
	}

	if (entry.is_luks)
	{
	    s << "is-luks:" << entry.is_luks;
	    if (!entry.luks_uuid.empty())
		s << " luks-uuid:" << entry.luks_uuid;
	    if (!entry.luks_label.empty())
		s << " luks-label:" << entry.luks_label;
	}

	if (entry.is_bitlocker)
	{
	    s << "is-bitlocker:" << entry.is_bitlocker;
	}

	if (entry.is_bcache)
	{
	    s << "is-bcache:" << entry.is_bcache;
	    if (!entry.bcache_uuid.empty())
		s << " bcache-uuid:" << entry.bcache_uuid;
	}

	return s;
    }


    void
    CmdBlkidVersion::query_version()
    {
	if (did_set_version)
	    return;

	SystemCmd cmd({ BLKID_BIN, "--version" }, SystemCmd::DoThrow);
	if (cmd.stdout().empty())
	    ST_THROW(SystemCmdException(&cmd, "failed to query blkid version"));

	parse_version(cmd.stdout()[0]);
    }


    void
    CmdBlkidVersion::parse_version(const string& version)
    {
	// example versions: "2.41.3"
	const regex version_rx("blkid from util-linux ([0-9]+)\\.([0-9]+)(\\.([0-9]+))?.*",
			       regex::extended);

	smatch match;

	if (!regex_match(version, match, version_rx))
	    ST_THROW(Exception("failed to parse blkid version '" + version + "'"));

	major = stoi(match[1]);
	minor = stoi(match[2]);
	patchlevel = match[4].length() == 0 ? 0 : stoi(match[4]);

	y2mil("major:" << major << " minor:" << minor << " patchlevel:" << patchlevel);

	did_set_version = true;
    }


    bool
    CmdBlkidVersion::supports_json_option_v2()
    {
	query_version();

	// Format of JSON output is supposed to change. Thus this function is called _v2.

	return false;
    }


    bool CmdBlkidVersion::did_set_version = false;

    int CmdBlkidVersion::major = 0;
    int CmdBlkidVersion::minor = 0;
    int CmdBlkidVersion::patchlevel = 0;

}
