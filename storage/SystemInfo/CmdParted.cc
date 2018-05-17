/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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


#include <fstream>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdParted.h"
#include "storage/Utils/Enum.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{
    using namespace std;


    Parted::Parted(const string& device)
	: device(device), label(PtType::UNKNOWN), region(), implicit(false),
	  gpt_undersized(false), gpt_backup_broken(false), gpt_pmbr_boot(false),
	  logical_sector_size(0), physical_sector_size(0)
    {
	SystemCmd::Options options(PARTEDBIN " --script --machine " + quote(device) +
				   " unit s print");
	options.throw_behaviour = SystemCmd::DoThrow;
	options.verify = [](int) { return true; };

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

		for ( const string& line: stderr )
		{
		    y2war( "parted stderr> " + line );
		}
	    }
	}

	parse(cmd.stdout(), cmd.stderr());
    }


    void
    Parted::parse(const vector<string>& stdout, const vector<string>& stderr)
    {
	implicit = false;
	gpt_undersized = false;
	gpt_backup_broken = false;
	gpt_pmbr_boot = false;
	entries.clear();

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

	scan_stderr(stderr);

	fix_dasd_sector_size();

	sort(entries.begin(), entries.end(), [](const Entry& lhs, const Entry& rhs)
	    { return lhs.number < rhs.number; }
	);

	y2mil(*this);
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
	PtType partition_type = PtType::UNKNOWN;

	if (s == "msdos")
	    partition_type = PtType::MSDOS;
	else if (s == "gpt" || s == "gpt_sync_mbr")
	    partition_type = PtType::GPT;
	else if (s == "dasd")
	    partition_type = PtType::DASD;
	else if (s == "loop")
	    partition_type = PtType::LOOP;
	else if (s == "unknown")
	    partition_type = PtType::UNKNOWN;
	else
	    ST_THROW(Exception("unknown partition table type reported by parted"));

	return partition_type;
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

	scan_entry_flags(tmp[6], entry);

	entries.push_back(entry);
    }


    void
    Parted::scan_entry_flags(const string& s, Entry& entry) const
    {
	entry.type = PartitionType::PRIMARY;

	// TODO parted has a strange interface to represent partition type
	// ids. On GPT it is not possible to distinguish whether the id is
	// linux or unknown. Work with upsteam parted to improve the
	// interface. The line below should then be entry.id = ID_UNKNOWN

	entry.id = ID_LINUX;

	entry.boot = false;
	entry.legacy_boot = false;

	vector<string> flags;
	boost::split(flags, s, boost::is_any_of(", "), boost::token_compress_on);

	if (contains(flags, "raid"))
	    entry.id = ID_RAID;
	else if (contains(flags, "lvm"))
	    entry.id = ID_LVM;
	else if (contains(flags, "prep"))
	    entry.id = ID_PREP;
	else if (contains(flags, "esp"))
	    entry.id = ID_ESP;
	else if (contains(flags, "swap"))
	    entry.id = ID_SWAP;
	else if (contains(flags, "bios_grub"))
	    entry.id = ID_BIOS_BOOT;
	else if (contains(flags, "msftdata"))
	    entry.id = ID_WINDOWS_BASIC_DATA;
	else if (contains(flags, "msftres"))
	    entry.id = ID_MICROSOFT_RESERVED;
	else if (contains(flags, "diag"))
	    entry.id = ID_DIAG;

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


    void
    Parted::scan_stderr(const vector<string>& stderr)
    {
	gpt_undersized = find_if(stderr, string_contains("fix the GPT to use all")) != stderr.end();

	gpt_backup_broken = find_if(stderr, string_contains("backup GPT table is corrupt, but the "
							    "primary appears OK")) != stderr.end();
    }


    void
    Parted::fix_dasd_sector_size()
    {
	if (label == PtType::DASD && logical_sector_size == 512 && physical_sector_size == 4096)
	{
	    y2mil("fixing sector size reported by parted");

	    region.set_length(region.get_length() / 8);
	    region.set_block_size(region.get_block_size() * 8);

	    for (Entry& entry : entries)
	    {
		Region& region = entry.region;

		region.set_start(region.get_start() / 8);
		region.set_length(region.get_length() / 8);
		region.set_block_size(region.get_block_size() * 8);
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
	s << "device:" << parted.device << " label:" << toString(parted.label)
	  << " region:" << parted.region;

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
	  << toString(entry.type) << " id:" << sformat("0x%02X", entry.id);

	if (entry.boot)
	    s << " boot";

	if (entry.legacy_boot)
	    s << " legacy-boot";

	return s;
    }

}
