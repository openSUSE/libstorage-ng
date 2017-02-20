/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
	: device(device), label(PtType::PT_UNKNOWN), region(), implicit(false),
	  gpt_enlarge(false), gpt_pmbr_boot(false)
    {
	SystemCmd cmd(PARTEDBIN " --script " + quote(device) + " unit s print", SystemCmd::DoThrow);

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
	gpt_enlarge = false;
	gpt_fix_backup = false;
	gpt_pmbr_boot = false;
	entries.clear();

	vector<string>::const_iterator pos;

	pos = find_if(stdout, string_starts_with("Partition Table:"));
	if (pos != stdout.end())
	{
	    string label_str = extractNthWord(2, *pos);
	    if (label_str == "msdos")
		label = PtType::MSDOS;
	    else if (label_str == "gpt" || label_str == "gpt_sync_mbr")
		label = PtType::GPT;
	    else if (label_str == "dasd")
		label = PtType::DASD;
	    else if (label_str == "loop")
		label = PtType::PT_LOOP;
	    else if (label_str == "unknown")
		label = PtType::PT_UNKNOWN;
	    else
		throw runtime_error("unknown partition table type");
	}
	else
	    y2war("could not find partition table");

	pos = find_if(stdout, string_starts_with("Disk " + device + ":"));
	if (pos != stdout.end())
	    scanDiskSize(*pos);
	else
	    y2war("could not find disk size");

	// not present for unrecognised disk label
	pos = find_if(stdout, string_starts_with("Sector size (logical/physical):"));
	if (pos != stdout.end())
	    scanSectorSizeLine(*pos);
	else
	    y2war("could not find sector size");

	pos = find_if(stdout, string_starts_with("Disk Flags:"));
	if (pos != stdout.end())
	    scanDiskFlags(*pos);
	else
	    y2war("could not find disk flags");

	gpt_enlarge = find_if(stderr, string_contains("fix the GPT to use all")) != stderr.end();
	gpt_fix_backup = find_if(stderr, string_contains("backup GPT table is corrupt, but the "
							 "primary appears OK")) != stderr.end();

	if (label != PtType::PT_UNKNOWN && label != PtType::PT_LOOP)
	{
	    int n = 0;

	    // Parse partition tables: One with cylinder sizes, one with sector sizes

	    for (const string& line : stdout)
	    {
		if (boost::starts_with(line, "Number"))
		    n++;

		string tmp = extractNthWord(0, line);
		if (!tmp.empty() && isdigit(tmp[0]))
		{
		    if (n == 1)
			scanSectorEntryLine(line);
		    else
			ST_THROW( ParseException( string( "Unexpected partition table #" )
						  + std::to_string(n), "", "" ) );
		}
	    }
	}

	y2mil(*this);
    }


    bool
    Parted::getEntry(unsigned num, Entry& entry) const
    {
	for (const_iterator it = entries.begin(); it != entries.end(); ++it)
	{
	    if (it->num == num)
	    {
		entry = *it;
		return true;
	    }
	}

	return false;
    }


    std::ostream&
    operator<<(std::ostream& s, const Parted& parted)
    {
	s << "device:" << parted.device << " label:" << toString(parted.label)
	  << " region:" << parted.region;

	if (parted.implicit)
	    s << " implicit";

	if (parted.gpt_enlarge)
	    s << " gpt-enlarge";

	if (parted.gpt_fix_backup)
	    s << " gpt-fix-backup";

	if (parted.gpt_pmbr_boot)
	    s << " gpt-pmbr-boot";

	s << '\n';

	for (Parted::const_iterator it = parted.entries.begin(); it != parted.entries.end(); ++it)
	    s << *it << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const Parted::Entry& entry)
    {
	s << "num:" << entry.num << " region:"
	  << entry.region << " type:" << toString(entry.type) << " id:" << entry.id;

	if (entry.boot)
	    s << " boot";

	if (entry.legacy_boot)
	    s << " legacy-boot";

	return s;
    }


    void
    Parted::scanDiskSize(const string& line)
    {
	string tmp(line);
	tmp.erase(0, tmp.find(':') + 1);

	int num_sectors = 0;
	tmp >> num_sectors;
	region.set_length(num_sectors);
    }


    void
    Parted::scanDiskFlags(const string& line)
    {
	implicit = boost::contains(line, "implicit_partition_table");
	gpt_pmbr_boot = boost::contains(line, "pmbr_boot");
    }


    void
    Parted::scanSectorSizeLine(const string& line)
    {
	// FIXME: This parser is too minimalistic and allows too much illegal input.
	// It turned out to be near impossible to come up with any test case that
	// actually made it throw an exception and not just silently do something random.
	// -- shundhammer 2015-05-13

	string tmp(line);
	tmp.erase(0, tmp.find(':') + 1);
	tmp = extractNthWord(0, tmp);

	list<string> l = splitString(extractNthWord(0, tmp), "/");

	if (l.size() == 2)
	{
	    list<string>::const_iterator i = l.begin();
	    int logical_sector_size = 0;
	    *i >> logical_sector_size;
	    region.set_block_size(logical_sector_size);
	}
	else
	{
	    ST_THROW( ParseException( "Bad sector size line", line,
				      "Sector size (logical/physical): 512B/4096B" ) );
	}
    }


    void
    Parted::scanSectorEntryLine(const string& line)
    {
	// Sample input:
	//
	//  1      2048s       4208639s     4206592s     primary   linux-swap(v1)  type=82
	//  2      4208640s    88100863s    83892224s    primary   btrfs           boot, type=83
	//  3      88100864s   171991039s   83890176s    primary   btrfs           type=83
	//  4      171991040s  3907028991s  3735037952s  extended                  lba, type=0f
	//  5      171993088s  3907008511s  3735015424s  logical   xfs             type=83
	//
	// (Number) (Start)    (End)        (Size)       (Type)    (File system)   (Flags)

	Entry entry;

	std::istringstream Data(line);
	classic(Data);

	unsigned long start_sector = 0;
	unsigned long end_sector = 0;
	unsigned long size_sector = 0;
	string PartitionTypeStr;
	string skip;

	if (label == PtType::MSDOS)
	{
	    Data >> entry.num >> start_sector >> skip >> end_sector >> skip >> size_sector
		 >> skip >> PartitionTypeStr;
	}
	else
	{
	    Data >> entry.num >> start_sector >> skip >> end_sector >> skip >> size_sector
		 >> skip;
	}

	if ( Data.fail() )	// parse error?
	{
	    ST_THROW(ParseException("Bad sector-based partition entry", line,
				    "2  4208640s  88100863s  83892224s  primary  btrfs boot, type=83"));
	}

	y2mil("num:" << entry.num << " start-sector:" << start_sector << " end-sector:" <<
	      end_sector << " size-sector:" << size_sector);

	if (entry.num == 0)
	    ST_THROW(ParseException("Illegal partition number 0", line, ""));

	entry.region = Region(start_sector, size_sector, region.get_block_size());

	char c;
	string TInfo;
	Data.unsetf(ifstream::skipws);
	Data >> c;
	char last_char = ',';
	while( Data.good() && !Data.eof() )
	{
	    if ( !isspace(c) )
	    {
		TInfo += c;
		last_char = c;
	    }
	    else
	    {
		if ( last_char != ',' )
		{
		    TInfo += ",";
		    last_char = ',';
		}
	    }
	    Data >> c;
	}

	boost::to_lower(TInfo, locale::classic());
	list<string> flags = splitString(TInfo, ",");
	y2mil("TInfo:" << TInfo << " flags:" << flags);

	// TODO parted has a strange interface to represent partition type
	// ids. E.g. swap is not displayed based on the id but the partition
	// content. On GPT it is also not possible to distinguish whether the
	// id is linux or unknown. Work with upsteam parted to improve the
	// interface. The line below should then be entry.id = ID_UNKNOWN.

	entry.id = ID_LINUX;

	if (label == PtType::MSDOS)
	{
	    if (PartitionTypeStr == "extended")
	    {
		entry.type = PartitionType::EXTENDED;
		entry.id = ID_EXTENDED;
	    }
	    else if (entry.num >= 5)
	    {
		entry.type = PartitionType::LOGICAL;
	    }
	}
	else if (contains_if(flags, string_starts_with("fat")))
	{
	    entry.id = ID_DOS32;
	}
	else if (contains(flags, "ntfs"))
	{
	    entry.id = ID_NTFS;
	}
	else if (contains_if(flags, string_contains("swap")))
	{
	    entry.id = ID_SWAP;
	}
	else if (contains(flags, "raid"))
	{
	    entry.id = ID_RAID;
	}
	else if (contains(flags, "lvm"))
	{
	    entry.id = ID_LVM;
	}
	else if (contains(flags, "prep"))
	{
	    entry.id = ID_PREP;
	}
	else if (contains(flags, "esp"))
	{
	    entry.id = ID_ESP;
	}

	if (label == PtType::MSDOS)
	{
	    entry.boot = contains(flags, "boot");

	    list<string>::const_iterator it1 = find_if(flags.begin(), flags.end(),
						       string_starts_with("type="));
	    if (it1 != flags.end())
	    {
		string val = string(*it1, 5);

		int tmp_id = 0;
		std::istringstream Data2(val);
		classic(Data2);
		Data2 >> std::hex >> tmp_id;
		if (tmp_id > 0)
		{
		    entry.id = tmp_id;
		}
	    }
	}

	if (label == PtType::GPT)
	{
	    entry.legacy_boot = contains(flags, "legacy_boot");

	    if (contains(flags, "bios_grub"))
	    {
		entry.id = ID_BIOS_BOOT;
	    }
	    else if (contains(flags, "msftdata"))
	    {
		entry.id = ID_WINDOWS_BASIC_DATA;
	    }
	    else if (contains(flags, "msftres"))
	    {
		entry.id = ID_MICROSOFT_RESERVED;
	    }
	}

	y2mil("num:" << entry.num << " region:" << entry.region << " id:" << entry.id <<
	      " type:" << toString(entry.type));

	entries.push_back(entry);
    }
}
