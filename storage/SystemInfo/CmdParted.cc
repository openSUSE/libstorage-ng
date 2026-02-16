/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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


#include <regex>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdParted.h"
#include "storage/Utils/Enum.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Utils/Format.h"
#include "storage/EnvironmentImpl.h"


namespace storage
{
    using namespace std;


    Parted::Parted(Udevadm& udevadm, const string& device)
	: device(device)
    {
	const bool json = CmdPartedVersion::supports_json_option();

	SystemCmd::Options options({ PARTED_BIN, "--script", json ? "--json" : "--machine", device,
		"unit", "s", "print" }, SystemCmd::DoThrow);
	options.verify = [](int) { return true; };
	if (!json)
	    options.setenv("PARTED_PRINT_NUMBER_OF_PARTITION_SLOTS", "1");

	SystemCmd cmd(options);

	// No check for exit status since parted 3.1 exits with 1 if no
	// partition table is found.

	if ( !cmd.stderr().empty() )
	{
	    this->stderr = cmd.stderr(); // Save stderr output

	    if ( boost::starts_with( cmd.stderr().front(), "Error: Could not stat device" ) )
		ST_THROW( SystemCmdException( &cmd, "parted complains: " + cmd.stderr().front() ) );
	    else
	    {
		// Intentionally NOT throwing an exception here for just any kind
		// of stderr output because it's quite common for the parted
		// command to write messages to stderr in certain situations that
		// may not necessarily be fatal.
		//
		// See also bsc#938572, bsc#938561

		for (const string& line : stderr)
		{
		    y2war( "parted stderr> " + line );
		}
	    }
	}

	parse(cmd.stdout(), cmd.stderr());

	if (CmdPartedVersion::print_triggers_udev())
	    udevadm.set_settle_needed();
    }


    void
    Parted::parse(const vector<string>& stdout, const vector<string>& stderr)
    {
	primary_slots = -1;
	implicit = false;
	gpt_undersized = false;
	gpt_backup_broken = false;
	gpt_pmbr_boot = false;
	entries.clear();

	if (CmdPartedVersion::supports_json_option())
	{
	    JsonFile json_file(stdout);

	    json_object* tmp1;
	    if (!get_child_node(json_file.get_root(), "disk", tmp1))
		ST_THROW(Exception("\"disk\" not found in json output of 'parted'"));

	    scan_device(tmp1);

	    if (label != PtType::UNKNOWN && label != PtType::LOOP)
	    {
		vector<json_object*> tmp2;
		if (!get_child_nodes(tmp1, "partitions", tmp2))
		    ST_THROW(Exception("\"partitions\" not found in json output of 'parted'"));

		for (json_object* tmp3 : tmp2)
		    scan_entry(tmp3);
	    }
	}
	else
	{
	    if (stdout.size() < 2)
		ST_THROW(Exception("wrong number of lines"));

	    if (stdout[0] != "BYT;")
		ST_THROW(ParseException("Bad first line", stdout[0], "BYT;"));

	    scan_device_line(stdout[1]);

	    if (label != PtType::UNKNOWN && label != PtType::LOOP)
	    {
		for (size_t i = 2; i < stdout.size(); ++i)
		    scan_entry_line(stdout[i]);
	    }
	}

	scan_stderr(stderr);

	fix_dasd_sector_size();

	sort(entries.begin(), entries.end(), [](const Entry& lhs, const Entry& rhs)
	    { return lhs.number < rhs.number; }
	);

	y2mil(*this);
    }


    void
    Parted::scan_device(json_object* node)
    {
	string tmp;

	unsigned long long num_sectors = 0;
	if (get_child_value(node, "size", tmp))
	    num_sectors = scan_sectors(tmp);

	get_child_value(node, "logical-sector-size", logical_sector_size);
	get_child_value(node, "physical-sector-size", physical_sector_size);

	region.set_length(num_sectors);
	region.set_block_size(logical_sector_size);

	if (get_child_value(node, "label", tmp))
	    label = scan_partition_table_type(tmp);

	scan_device_flags(node);

	get_child_value(node, "max-partitions", primary_slots);
    }


    void
    Parted::scan_device_flags(json_object* node)
    {
	vector<json_object*> nodes;
	if (!get_child_nodes(node, "flags", nodes))
	    return;

	for (json_object* node : nodes)
	{
	    if (!json_object_is_type(node, json_type_string))
		continue;

	    string flag = json_object_get_string(node);

	    if (flag == "pmbr_boot")
		gpt_pmbr_boot = true;
	    else if (flag == "implicit_partition_table")
		implicit = true;
	}
    }


    void
    Parted::scan_entry(json_object* node)
    {
	Entry entry;

	if (!get_child_value(node, "number", entry.number))
	    ST_THROW(Exception("number not found"));

	if (entry.number == 0)
	    ST_THROW(Exception("illegal partition number 0"));

	string tmp;

	unsigned long long start_sector = 0;
	if (get_child_value(node, "start", tmp))
	    start_sector = scan_sectors(tmp);

	unsigned long long size_sector = 0;
	if (get_child_value(node, "size", tmp))
	    size_sector = scan_sectors(tmp);

	entry.region = Region(start_sector, size_sector, region.get_block_size());

	if (label == PtType::MSDOS)
	{
	    if (get_child_value(node, "type", tmp))
		toValue(tmp, entry.type);
	}

	if (label == PtType::GPT)
	{
	    get_child_value(node, "name", entry.name);
	}

	scan_entry_flags(node, entry);

	if (label == PtType::MSDOS)
	{
	    // The "type-id" entry is SUSE specific (2022-04-22). Meanwhile included
	    // upstream (2022-05-24).

	    if (get_child_value(node, "type-id", tmp))
	    {
		std::istringstream data(tmp);
		classic(data);
		data >> std::hex >> entry.id;
	    }
	}

	if (label == PtType::GPT)
	{
	    // ID_LINUX is the default and thus also the value for unknown types. In that
	    // case look for the type UUID. The "type-uuid" is meanwhile included upstream
	    // (2022-05-24).

	    if (entry.id == ID_LINUX && get_child_value(node, "type-uuid", tmp))
	    {
		map<unsigned int, const char*>::const_iterator it =
		    find_if(id_to_uuid.begin(), id_to_uuid.end(),
			    [&tmp](const auto& v) { return v.second == tmp; });

		entry.id = it != id_to_uuid.end() ? it->first : ID_UNKNOWN;
	    }
	}

	entries.push_back(entry);
    }


    void
    Parted::scan_entry_flags(json_object* node, Entry& entry) const
    {
	entry.id = ID_LINUX;
	entry.boot = false;
	entry.legacy_boot = false;
	entry.no_automount = false;

	vector<json_object*> nodes;
	if (!get_child_nodes(node, "flags", nodes))
	    return;

	for (json_object* node : nodes)
	{
	    if (!json_object_is_type(node, json_type_string))
		continue;

	    string flag = json_object_get_string(node);

	    for (map<unsigned int, const char*>::value_type tmp : id_to_name)
	    {
		if (flag == tmp.second)
		{
		    entry.id = tmp.first;
		    break;
		}
	    }

	    switch (label)
	    {
		case PtType::MSDOS:
		    if (flag == "boot")
			entry.boot = true;
		    break;

		case PtType::GPT:
		    if (flag == "legacy_boot")
			entry.legacy_boot = true;
		    else if (flag == "no_automount")
			entry.no_automount = true;
		    break;

		default:
		    break;
	    }
	}
    }


    void
    Parted::scan_device_line(const string& line)
    {
	vector<string> tmp = tokenize(line);

	unsigned long long num_sectors = 0;
	tmp[1] >> num_sectors;

	region.set_length(num_sectors);

	tmp[3] >> logical_sector_size;
	tmp[4] >> physical_sector_size;

	region.set_block_size(logical_sector_size);

	label = scan_partition_table_type(tmp[5]);

	scan_device_flags(tmp[7]);

	if (tmp.size() >= 9)
	    tmp[8] >> primary_slots;
    }


    void
    Parted::scan_device_flags(const string& s)
    {
	implicit = boost::contains(s, "implicit_partition_table");

	gpt_pmbr_boot = boost::contains(s, "pmbr_boot");
    }


    PtType
    Parted::scan_partition_table_type(const string& s) const
    {
	for (map<const char*, PtType>::value_type tmp : name_to_pt_type)
	{
	    if (tmp.first == s)
		return tmp.second;
	}

	return PtType::UNKNOWN;
    }


    void
    Parted::scan_entry_line(const string& line)
    {
	vector<string> tmp = tokenize(line);

	Entry entry;

	tmp[0] >> entry.number;

	if (entry.number == 0)
	    ST_THROW(ParseException("Illegal partition number 0", line, ""));

	unsigned long long start_sector = 0;
	unsigned long long size_sector = 0;

	tmp[1] >> start_sector;
	tmp[3] >> size_sector;

	entry.region = Region(start_sector, size_sector, region.get_block_size());

	entry.name = tmp[5];

	scan_entry_flags(tmp[6], entry);

	entries.push_back(entry);
    }


    void
    Parted::scan_entry_flags(const string& s, Entry& entry) const
    {
	entry.type = PartitionType::PRIMARY;

	// TODO parted has a strange interface to represent partition type
	// ids. On GPT it is not possible to distinguish whether the id is
	// linux or unknown. Work with upstream parted to improve the
	// interface. The line below should then be entry.id = ID_UNKNOWN

	entry.id = ID_LINUX;

	entry.boot = false;
	entry.legacy_boot = false;
	entry.no_automount = false;

	vector<string> flags;
	boost::split(flags, s, boost::is_any_of(", "), boost::token_compress_on);

	for (map<unsigned int, const char*>::value_type tmp : id_to_name)
	{
	    if (contains(flags, tmp.second))
	    {
		entry.id = tmp.first;
		break;
	    }
	}

	if (label == PtType::MSDOS)
	{
	    entry.boot = contains(flags, "boot");

	    vector<string>::const_iterator it1 = find_if(flags.begin(), flags.end(),
							 string_starts_with("type="));
	    if (it1 != flags.end())
	    {
		string val = string(*it1, 5);

		int id = 0;
		std::istringstream data(val);
		classic(data);
		data >> std::hex >> id;
		if (id > 0)
		    entry.id = id;
	    }

	    if (entry.number > 4)
		entry.type = PartitionType::LOGICAL;
	    // see https://github.com/torvalds/linux/blob/master/include/linux/genhd.h#L32
	    else if (contains(vector<unsigned int>({ 0x05, 0x0f, 0x85 }), entry.id))
		entry.type = PartitionType::EXTENDED;
	}

	if (label == PtType::GPT)
	{
	    entry.legacy_boot = contains(flags, "legacy_boot");
	}
    }


    unsigned long long
    Parted::scan_sectors(const string& s) const
    {
	if (s.empty() || s.back() != 's')
	    ST_THROW(ParseException("bad sector value", s, "1024s"));

	unsigned long long ret;

	std::istringstream istr(s);
	classic(istr);
	istr >> ret;

	return ret;
    }


    void
    Parted::scan_stderr(const vector<string>& stderr)
    {
	gpt_undersized = contains_if(stderr, string_contains("fix the GPT to use all"));

	gpt_backup_broken = contains_if(stderr, string_contains("backup GPT table is corrupt, but the "
								"primary appears OK"));
    }


    void
    Parted::fix_dasd_sector_size()
    {
	// see do_resize() and do_create() in PartitionImpl.cc
	if (label == PtType::DASD && logical_sector_size == 512 &&
	    (physical_sector_size == 4096 || physical_sector_size == 1024 || physical_sector_size == 2048))
	{
	    int factor = physical_sector_size / logical_sector_size;
	    y2mil("fixing sector size reported by parted by factor:" << factor);

	    region.set_length(region.get_length() / factor);
	    region.set_block_size(region.get_block_size() * factor);

	    for (Entry& entry : entries)
	    {
		Region& region = entry.region;

		region.set_start(region.get_start() / factor);
		region.set_length(region.get_length() / factor);
		region.set_block_size(region.get_block_size() * factor);
	    }
	}
    }


    bool
    Parted::get_entry(unsigned number, Entry& entry) const
    {
	for (const_iterator it = entries.begin(); it != entries.end(); ++it)
	{
	    if (it->number == number)
	    {
		entry = *it;
		return true;
	    }
	}

	return false;
    }


    vector<string>
    Parted::tokenize(const string& line) const
    {
	if (!boost::ends_with(line, ";"))
	    ST_THROW(ParseException("missing semicolon", "", ";"));

	string line_without_semicolon = line.substr(0, line.size() - 1);

	vector<string> ret;

	string tmp;

	// see bsc #1066467

	string::const_iterator it = line_without_semicolon.begin();
	while (true)
	{
	    if (*it == ':')
	    {
		ret.push_back(tmp);
		tmp.clear();
	    }
	    else if (*it == '\\')
	    {
		if (++it == line_without_semicolon.end())
		    ST_THROW(Exception("premature end-of-string"));

		tmp += *it;
	    }
	    else
	    {
		tmp += *it;
	    }

	    if (++it == line_without_semicolon.end())
	    {
		ret.push_back(tmp);
		break;
	    }
	}

	return ret;
    }


    std::ostream&
    operator<<(std::ostream& s, const Parted& parted)
    {
	s << "device:" << parted.device << " label:" << get_pt_type_name(parted.label)
	  << " region:" << parted.region;

	if (parted.primary_slots >= 0)
	    s << " primary-slots:" << parted.primary_slots;

	if (parted.implicit)
	    s << " implicit";

	if (parted.gpt_undersized)
	    s << " gpt-undersized";

	if (parted.gpt_backup_broken)
	    s << " gpt-backup-broken";

	if (parted.gpt_pmbr_boot)
	    s << " gpt-pmbr-boot";

	s << '\n';

	for (const Parted::Entry& entry : parted.entries)
	    s << entry << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const Parted::Entry& entry)
    {
	s << "number:" << entry.number << " region:" << entry.region << " type:"
	  << get_partition_type_name(entry.type) << " id:" << sformat("0x%02x", entry.id);

	if (entry.boot)
	    s << " boot";

	if (entry.legacy_boot)
	    s << " legacy-boot";

	if (entry.no_automount)
	    s << " no-automount";

	if (!entry.name.empty())
	    s << " name:" << entry.name;

	return s;
    }


    const map<unsigned int, const char*> Parted::id_to_name = {
	{ ID_BIOS_BOOT, "bios_grub" },
	{ ID_DIAG, "diag" },
	{ ID_ESP, "esp" },
	{ ID_IRST, "irst" },
	{ ID_LINUX_HOME, "linux-home" },
	{ ID_LVM, "lvm" },
	{ ID_MICROSOFT_RESERVED, "msftres" },
	{ ID_PREP, "prep" },
	{ ID_RAID, "raid" },
	{ ID_SWAP, "swap" },
	{ ID_WINDOWS_BASIC_DATA, "msftdata" },
    };


    const map<unsigned int, const char*> Parted::id_to_uuid = {
	{ ID_LINUX, "0fc63daf-8483-4772-8e79-3d69d8477de4" },
	{ ID_LINUX_ROOT_ARM, "69dad710-2ce4-4e3c-b16c-21a1d49abed3" },
	{ ID_LINUX_ROOT_AARCH64, "b921b045-1df0-41c3-af44-4c6f280d3fae" },
	{ ID_LINUX_ROOT_PPC32, "1de3f1ef-fa98-47b5-8dcd-4a860a654d78" },
	{ ID_LINUX_ROOT_PPC64BE, "912ade1d-a839-4913-8964-a10eee08fbd2" },
	{ ID_LINUX_ROOT_PPC64LE, "c31c45e6-3f39-412e-80fb-4809c4980599" },
	{ ID_LINUX_ROOT_RISCV32, "60d5a7fe-8e7d-435c-b714-3dd8162144e1" },
	{ ID_LINUX_ROOT_RISCV64, "72ec70a6-cf74-40e6-bd49-4bda08e8f224" },
	{ ID_LINUX_ROOT_S390, "08a7acea-624c-4a20-91e8-6e0fa67d23f9" },
	{ ID_LINUX_ROOT_S390X, "5eead9a9-fe09-4a1e-a1d7-520d00531306" },
	{ ID_LINUX_ROOT_X86, "44479540-f297-41b2-9af7-d131d5f0458a" },
	{ ID_LINUX_ROOT_X86_64, "4f68bce3-e8cd-4db1-96e7-fbcaf984b709" },
	{ ID_LINUX_SERVER_DATA, "3b8f8425-20e0-4f3b-907f-1a25a76f98e8" },
	{ ID_LINUX_USR_ARM, "7d0359a3-02b3-4f0a-865c-654403e70625" },
	{ ID_LINUX_USR_AARCH64, "b0e01050-ee5f-4390-949a-9101b17104e9" },
	{ ID_LINUX_USR_PPC32, "7d14fec5-cc71-415d-9d6c-06bf0b3c3eaf" },
	{ ID_LINUX_USR_PPC64BE, "2c9739e2-f068-46b3-9fd0-01c5a9afbcca" },
	{ ID_LINUX_USR_PPC64LE, "15bb03af-77e7-4d4a-b12b-c0d084f7491c" },
	{ ID_LINUX_USR_RISCV32, "b933fb22-5c3f-4f91-af90-e2bb0fa50702" },
	{ ID_LINUX_USR_RISCV64, "beaec34b-8442-439b-a40b-984381ed097d" },
	{ ID_LINUX_USR_S390, "cd0f869b-d0fb-4ca0-b141-9ea87cc78d66" },
	{ ID_LINUX_USR_S390X, "8a4f5770-50aa-4ed3-874a-99b710db6fea" },
	{ ID_LINUX_USR_X86, "75250d76-8cc6-458e-bd66-bd47cc81a812" },
	{ ID_LINUX_USR_X86_64, "8484680c-9521-48c6-9c11-b0720656f69e" },
	{ ID_SWAP, "0657fd6d-a4ab-43c4-84e5-0933c84b4f4f" },
    };


    const map<const char*, PtType> Parted::name_to_pt_type = {
	{ "aix", PtType::AIX },
	{ "amiga", PtType::AMIGA },
	{ "atari", PtType::ATARI },
	{ "bsd", PtType::BSD },
	{ "dasd", PtType::DASD },
	{ "dvh", PtType::DVH },
	{ "gpt", PtType::GPT },
	{ "gpt_sync_mbr", PtType::GPT },
	{ "implicit", PtType::IMPLICIT },
	{ "loop", PtType::LOOP },
	{ "mac", PtType::MAC },
	{ "msdos", PtType::MSDOS },
	{ "sun", PtType::SUN },
	{ "unknown", PtType::UNKNOWN },
    };


    void
    CmdPartedVersion::query_version()
    {
	if (did_set_version)
	    return;

	SystemCmd cmd({ PARTED_BIN, "--version" }, SystemCmd::DoThrow);
	if (cmd.stdout().empty())
	    ST_THROW(SystemCmdException(&cmd, "failed to query parted version"));

	parse_version(cmd.stdout()[0]);
    }


    void
    CmdPartedVersion::parse_version(const string& version)
    {
	// example versions: "3.4", "3.5", "3.5.1", "3.5.1-cec5"
	const regex version_rx("parted \\(GNU parted\\) ([0-9]+)\\.([0-9]+)(\\.([0-9]+)(-([0-9a-z]+))?)?",
			       regex::extended);

	smatch match;

	if (!regex_match(version, match, version_rx))
	    ST_THROW(Exception("failed to parse parted version '" + version + "'"));

	major = stoi(match[1]);
	minor = stoi(match[2]);
	patchlevel = match[4].length() == 0 ? 0 : stoi(match[4]);

	y2mil("major:" << major << " minor:" << minor << " patchlevel:" << patchlevel);

	did_set_version = true;
    }


    bool
    CmdPartedVersion::supports_json_option()
    {
	query_version();

	return major >= 4 || (major == 3 && minor >= 5);
    }


    bool
    CmdPartedVersion::supports_type_command()
    {
	query_version();

	if (os_flavour() == OsFlavour::SUSE)
	    return major >= 4 || (major == 3 && minor >= 5);

	return major >= 4 || (major == 3 && minor >= 6);
    }


    bool
    CmdPartedVersion::supports_old_type_flag()
    {
	query_version();

	return (major == 3 && minor < 5) && os_flavour() == OsFlavour::SUSE;
    }


    bool
    CmdPartedVersion::supports_wipe_signatures()
    {
	// Option --wipesignatures is not available in upstream parted (2021-07-26).

	return os_flavour() == OsFlavour::SUSE;
    }


    bool
    CmdPartedVersion::supports_ignore_busy()
    {
	// Option --ignore-busy is not available in upstream parted (2021-07-26).

	return os_flavour() == OsFlavour::SUSE;
    }


    bool
    CmdPartedVersion::print_triggers_udev()
    {
	// SUSE has a patch to open the device read-only if only e.g. print is executed.

	return os_flavour() != OsFlavour::SUSE;
    }


    bool
    CmdPartedVersion::supports_no_automount_flag()
    {
	query_version();

	return major >= 4 || (major == 3 && minor >= 6);
    }


    bool CmdPartedVersion::did_set_version = false;

    int CmdPartedVersion::major = 0;
    int CmdPartedVersion::minor = 0;
    int CmdPartedVersion::patchlevel = 0;

}
