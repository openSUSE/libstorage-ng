/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2017-2023] SUSE LLC
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

#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/JsonFile.h"
#include "storage/SystemInfo/CmdBtrfs.h"
#include "storage/Filesystems/BtrfsImpl.h"


namespace storage
{
    using namespace std;


    CmdBtrfsFilesystemShow::CmdBtrfsFilesystemShow(Udevadm& udevadm)
    {
	udevadm.settle();

	SystemCmd::Options cmd_options({ BTRFS_BIN, "filesystem", "show" }, SystemCmd::DoThrow);
	cmd_options.verify = [](int) { return true; };

	SystemCmd cmd(cmd_options);

	if ( cmd.retcode() == 0 && !cmd.stdout().empty() )
	    parse( cmd.stdout() );
	else if ( ! cmd.stderr().empty() )
	    ST_THROW( SystemCmdException( &cmd, "'btrfs filesystem show' complains: "
					  + cmd.stderr().front() ) );

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


    CmdBtrfsFilesystemShow::const_iterator
    CmdBtrfsFilesystemShow::find_by_uuid(const string& uuid) const
    {
	return find_if(begin(), end(), [&uuid](const Entry& entry){ return entry.uuid == uuid; });
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
	const string tmp = BTRFS_BIN " subvolume list -a -puq ";
	SystemCmd::Options cmd_options(tmp + quote(mount_point), SystemCmd::DoThrow);
	cmd_options.mockup_key = tmp + "(device:" + key + ")";

	SystemCmd cmd(cmd_options);
	parse(cmd.stdout());
    }


    void
    CmdBtrfsSubvolumeList::parse(const vector<string>& lines)
    {
	for (const string& line : lines)
	{
	    Entry entry;

	    string::size_type pos1 = line.find("ID ");
	    if (pos1 == string::npos)
		ST_THROW(Exception("could not find 'id' in 'btrfs subvolume list' output"));
	    line.substr(pos1 + strlen("ID ")) >> entry.id;

	    string::size_type pos2 = line.find(" parent ");
	    if (pos2 == string::npos)
		ST_THROW(Exception("could not find 'parent' in 'btrfs subvolume list' output"));
	    line.substr(pos2 + strlen(" parent ")) >> entry.parent_id;

	    // Subvolume can already be deleted, in which case parent is "0"
	    // (and path "DELETED"). That is a temporary state.
	    if (entry.parent_id == 0)
		continue;

	    string::size_type pos3 = line.find(" path ");
	    if (pos3 == string::npos)
		ST_THROW(Exception("could not find 'path' in 'btrfs subvolume list' output"));
	    entry.path = line.substr(pos3 + strlen(" path "));
	    if (boost::starts_with(entry.path, "<FS_TREE>/"))
		entry.path.erase(0, strlen("<FS_TREE>/"));

	    string::size_type pos4 = line.find(" uuid ");
	    if (pos4 == string::npos)
		ST_THROW(Exception("could not find 'uuid' in 'btrfs subvolume list' output"));
	    line.substr(pos4 + strlen(" uuid ")) >> entry.uuid;

	    string::size_type pos5 = line.find(" parent_uuid ");
	    if (pos5 == string::npos)
		ST_THROW(Exception("could not find 'parent_uuid' in 'btrfs subvolume list' output"));
	    line.substr(pos5 + strlen(" parent_uuid ")) >> entry.parent_uuid;
	    if (entry.parent_uuid == "-")
		entry.parent_uuid = "";

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
	  << " path:" << entry.path << " uuid:" << entry.uuid;

	if (!entry.parent_uuid.empty())
	    s << " parent-uuid:" << entry.parent_uuid;

	s  << '\n';

	return s;
    }


    CmdBtrfsSubvolumeShow::CmdBtrfsSubvolumeShow(const key_t& key, const string& mount_point)
    {
	const string tmp = BTRFS_BIN " subvolume show ";
	SystemCmd::Options cmd_options(tmp + quote(mount_point), SystemCmd::DoThrow);
	cmd_options.mockup_key = tmp + "(device:" + key + ")";

	SystemCmd cmd(cmd_options);
	parse(cmd.stdout());
    }


    void
    CmdBtrfsSubvolumeShow::parse(const vector<string>& lines)
    {
	static const regex uuid_regex("[ \t]*UUID:[ \t]*(" UUID_REGEX "|-)[ \t]*", regex::extended);

	smatch match;

	for (const string& line : lines)
	{
	    if (regex_match(line, match, uuid_regex))
		uuid = match[1];
	}

	if (uuid.empty())
	    ST_THROW(Exception("could not find 'uuid' in 'btrfs subvolume show' output"));

	if (uuid == "-")
	{
	    // If the btrfs was created with older kernels (whatever that means) (tested
	    // with SLES 11 SP3), the top-level subvolume does not have a UUID. Other
	    // subvolumes do have a UUID. In that case also all subvolumes are listed
	    // wrongly as snapshots of the top-level subvolume (by 'btrfs subvolume
	    // show'). The relationship between other subvolumes/snapshots seems to be
	    // fine.

	    y2mil("could not find 'uuid' in 'btrfs subvolume show' output - happens if "
		  "btrfs was created with an old kernel");

	    uuid = "";
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsSubvolumeShow& cmd_btrfs_subvolume_show)
    {
	s << "uuid:" << cmd_btrfs_subvolume_show.uuid;

	return s;
    }


    CmdBtrfsSubvolumeGetDefault::CmdBtrfsSubvolumeGetDefault(const key_t& key, const string& mount_point)
    {
	const string tmp = BTRFS_BIN " subvolume get-default ";
	SystemCmd::Options cmd_options(tmp + quote(mount_point), SystemCmd::DoThrow);
	cmd_options.mockup_key = tmp + "(device:" + key + ")";

	SystemCmd cmd(cmd_options);
	parse(cmd.stdout());
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
    {
	const bool json = CmdBtrfsVersion::supports_json_option_for_filesystem_df();

	const string tmp = BTRFS_BIN " " + string(json ? "--format json " : "") + "filesystem df ";
	SystemCmd::Options cmd_options(tmp + quote(mount_point), SystemCmd::DoThrow);
	cmd_options.mockup_key = tmp + "(device:" + key + ")";

	SystemCmd cmd(cmd_options);
	if (json)
	    parse_json(cmd.stdout());
	else
	    parse(cmd.stdout());
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


    void
    CmdBtrfsFilesystemDf::parse_json(const vector<string>& lines)
    {
	JsonFile json_file(lines);

	vector<json_object*> tmp1;
	if (!get_child_nodes(json_file.get_root(), "filesystem-df", tmp1))
	    ST_THROW(Exception("\"filesystem-df\" not found in json output of 'btrfs filesystem df'"));

	for (json_object* tmp2 : tmp1)
	{
	    string tmp3, tmp4;

	    if (!get_child_value(tmp2, "bg-type", tmp3))
		ST_THROW(Exception("\"bg-type\" not found or invalid"));

	    if (!get_child_value(tmp2, "bg-profile", tmp4))
		ST_THROW(Exception("\"bg-profile\" not found or invalid"));

	    boost::to_upper(tmp4, locale::classic());

	    if (tmp3 == "Metadata")
		metadata_raid_level = toValueWithFallback(tmp4, BtrfsRaidLevel::UNKNOWN);
	    else if (tmp3 == "Data")
		data_raid_level = toValueWithFallback(tmp4, BtrfsRaidLevel::UNKNOWN);
	    else if (tmp3 == "Data+Metadata")
		metadata_raid_level = data_raid_level = toValueWithFallback(tmp4, BtrfsRaidLevel::UNKNOWN);
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


    CmdBtrfsQgroupShow::CmdBtrfsQgroupShow(const key_t& key, const string& mount_point)
    {
	// There is no btrfs command line way to just query if quota is enabled. So we
	// assume it is enabled if 'btrfs qgroup show' does not report an error.

	const bool json = CmdBtrfsVersion::supports_json_option_for_qgroup_show();

	const string tmp = BTRFS_BIN " " + string(json ? "--format json " : "") + "qgroup show -rep --raw ";
	SystemCmd::Options cmd_options(tmp + quote(mount_point), SystemCmd::DoThrow);
	cmd_options.mockup_key = tmp + "(device:" + key + ")";
	cmd_options.verify = [](int exit_code) { return exit_code == 0 || exit_code == 1; };

	SystemCmd cmd(cmd_options);
	if (cmd.retcode() == 0)
	{
	    quota = true;

	    if (json)
		parse_json(cmd.stdout());
	    else
		parse(cmd.stdout());
	}
    }


    void
    CmdBtrfsQgroupShow::parse(const vector<string>& lines)
    {
	// Output changed slightly between btrfsprogs 6.0 and 6.0.2. Handle both.

	for (const string& line : lines)
	{
	    const string line_trimmed = boost::trim_copy(line);

	    vector<string> columns;
	    boost::split(columns, line_trimmed, boost::is_any_of("\t "), boost::token_compress_on);

	    if (columns.size() < 6)
		ST_THROW(Exception("failed to parse qgroup output"));

	    if (columns[0] == "qgroupid" || columns[0] == "Qgroupid" || columns[0] == "--------")
		continue;

	    Entry entry;

	    entry.id = BtrfsQgroup::Impl::parse_id(columns[0]);

	    columns[1] >> entry.referenced;
	    columns[2] >> entry.exclusive;

	    if (columns[3] != "none")
		columns[3] >> entry.referenced_limit;

	    if (columns[4] != "none")
		columns[4] >> entry.exclusive_limit;

	    if (columns[5] != "---" && columns[5] != "-")
	    {
		vector<string> tmp;
		boost::split(tmp, columns[5], boost::is_any_of(","), boost::token_compress_on);

		for (const string& t : tmp)
		    entry.parents_id.push_back(BtrfsQgroup::Impl::parse_id(t));
	    }

	    data.push_back(entry);
	}

	y2mil(*this);
    }


    bool
    CmdBtrfsQgroupShow::parse_limit(json_object* parent, const char* name,
				    std::optional<unsigned long long>& value) const
    {
	// See JsonFile.cc for similar code.

	json_object* child;

	if (!json_object_object_get_ex(parent, name, &child))
	    return true;

	if (json_object_is_type(child, json_type_null))
	    return true;

	if (json_object_is_type(child, json_type_string) && strcmp(json_object_get_string(child), "none") == 0)
	    return true;

	if (!json_object_is_type(child, json_type_int) && !json_object_is_type(child, json_type_string))
	    return false;

	value = json_object_get_int64(child);

	return true;
    }


    void
    CmdBtrfsQgroupShow::parse_json(const vector<string>& lines)
    {
	JsonFile json_file(lines);

	vector<json_object*> tmp1;
	if (!get_child_nodes(json_file.get_root(), "qgroup-show", tmp1))
	    ST_THROW(Exception("\"qgroup-show\" not found in json output of 'btrfs qgroup show'"));

	for (json_object* tmp2 : tmp1)
	{
	    Entry entry;

	    string tmp3;

	    if (!get_child_value(tmp2, "qgroupid", tmp3))
		ST_THROW(Exception("\"qgroupid\" not found or invalid"));

	    entry.id = BtrfsQgroup::Impl::parse_id(tmp3);

	    if (!get_child_value(tmp2, "referenced", entry.referenced))
		ST_THROW(Exception("\"referenced\" not found or invalid"));

	    if (!get_child_value(tmp2, "exclusive", entry.exclusive))
		ST_THROW(Exception("\"exclusive\" not found or invalid"));

	    if (!parse_limit(tmp2, "max_referenced", entry.referenced_limit))
		ST_THROW(Exception("\"max_referenced\" not found or invalid"));

	    if (!parse_limit(tmp2, "max_exclusive", entry.exclusive_limit))
		ST_THROW(Exception("\"max_exclusive\" not found or invalid"));

	    vector<json_object*> tmp4;
	    if (!get_child_nodes(tmp2, "parents", tmp4))
		ST_THROW(Exception("\"parents\" not found or invalid"));

	    for (json_object* tmp5 : tmp4)
		entry.parents_id.push_back(BtrfsQgroup::Impl::parse_id(json_object_get_string(tmp5)));

	    data.push_back(entry);
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsQgroupShow& cmd_btrfs_qgroups_show)
    {
	for (const CmdBtrfsQgroupShow::Entry& entry : cmd_btrfs_qgroups_show)
	    s << entry;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdBtrfsQgroupShow::Entry& entry)
    {
	s << "id:" << BtrfsQgroup::Impl::format_id(entry.id);

	s << " referenced:" << entry.referenced << " exclusive:" << entry.exclusive;

	if (entry.referenced_limit)
	    s << " referenced-limit:" << entry.referenced_limit.value();

	if (entry.exclusive_limit)
	    s << " exclusive-limit:" << entry.exclusive_limit.value();

	if (!entry.parents_id.empty())
	{
	    s << " parents:";

	    for (vector<BtrfsQgroup::id_t>::const_iterator it = entry.parents_id.begin();
		 it != entry.parents_id.end(); ++it)
	    {
		if (it != entry.parents_id.begin())
		    s << ",";

		s << BtrfsQgroup::Impl::format_id(*it);
	    }
	}

	return s << '\n';
    }


    void
    CmdBtrfsVersion::query_version()
    {
	if (did_set_version)
	    return;

	SystemCmd cmd({ BTRFS_BIN, "--version" }, SystemCmd::DoThrow);
	if (cmd.stdout().empty())
	    ST_THROW(SystemCmdException(&cmd, "failed to query btrfs version"));

	parse_version(cmd.stdout()[0]);
    }


    void
    CmdBtrfsVersion::parse_version(const string& version)
    {
	// example versions: "5.14 " (yes, with a trailing space), "6.0", "6.0.2"
	const regex version_rx("btrfs-progs v([0-9]+)\\.([0-9]+)(\\.([0-9]+))?( )*", regex::extended);

	smatch match;

	if (!regex_match(version, match, version_rx))
	    ST_THROW(Exception("failed to parse btrfs version '" + version + "'"));

	major = stoi(match[1]);
	minor = stoi(match[2]);
	patchlevel = match[4].length() == 0 ? 0 : stoi(match[4]);

	y2mil("major:" << major << " minor:" << minor << " patchlevel:" << patchlevel);

	did_set_version = true;
    }


    bool
    CmdBtrfsVersion::supports_json_option_for_filesystem_df()
    {
	query_version();

	return major >= 7 || (major == 6 && minor >= 1);
    }


    bool
    CmdBtrfsVersion::supports_json_option_for_qgroup_show()
    {
	query_version();

	// buggy in 6.1, reported, assume fixed in 6.2 (2023-01-04)
	return major >= 7 || (major == 6 && minor >= 2);
    }


    bool CmdBtrfsVersion::did_set_version = false;

    int CmdBtrfsVersion::major = 0;
    int CmdBtrfsVersion::minor = 0;
    int CmdBtrfsVersion::patchlevel = 0;

}
