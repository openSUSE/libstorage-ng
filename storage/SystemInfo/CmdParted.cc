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


#include <fstream>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdParted.h"
#include "storage/Utils/Enum.h"
#include "storage/Devices/Partition.h"
#include "storage/Utils/StorageTypes.h"


namespace storage
{
    using namespace std;


    Parted::Parted(const string& device)
	: device(device), label(PtType::PT_UNKNOWN), implicit(false), gpt_enlarge(false)
    {
	SystemCmd cmd(PARTEDBIN " --script " + quote(device) + " unit cyl print unit s print", SystemCmd::DoThrow);

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

	// only present for unrecognised disk label due to patch in parted
	pos = find_if(stdout, string_starts_with("BIOS cylinder,head,sector geometry:"));
	if (pos != stdout.end())
	    scanGeometryLine(*pos);
	else
	{
	    ST_THROW( ParseException( "No disk geometry line",
				      "...", // don't pass complete parted output to exception
				      "BIOS cylinder,head,sector geometry:" ) );
	}
	// see bnc #866535
	pos = find_if(stdout, string_starts_with("Disk " + device + ":"));
	if (pos != stdout.end())
	{
	    unsigned long tmp;
	    extractNthWord(2, *pos) >> tmp;
	    if (geometry.cylinders != tmp)
	    {
		y2war("parted reported different cylinder numbers");
		geometry.cylinders = min(geometry.cylinders, tmp);
	    }
	}
	else
	    y2war("could not find cylinder number");

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
			scanCylEntryLine(line);
		    else if (n == 2)
			scanSecEntryLine(line);
		    else
			ST_THROW( ParseException( string( "Unexpected partition table #" )
						  + std::to_string(n), "", "" ) );
		}
	    }

	    iterator prev = entries.begin();
	    iterator it = prev;
	    if (it != entries.end())
		++it;
	    while (it != entries.end())
	    {
		if (prev->cylRegion.get_start() >= it->cylRegion.get_start() && it->cylRegion.get_length() > 0)
		{
		    y2mil("old:" << it->cylRegion);
		    it->cylRegion.set_start(it->cylRegion.get_start() + 1);
		    it->cylRegion.set_length(it->cylRegion.get_length() - 1);
		    y2mil("new:" << it->cylRegion);
		}

		prev = it;
		++it;
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


    std::ostream& operator<<(std::ostream& s, const Parted& parted)
    {
	s << "device:" << parted.device << " label:" << toString(parted.label)
	  << " geometry:" << parted.geometry;

	if (parted.implicit)
	    s << " implicit";

	if (parted.gpt_enlarge)
	    s << " gpt_enlarge";

	if (parted.gpt_fix_backup)
	    s << " gpt_fix_backup";

	s << endl;

	for (Parted::const_iterator it = parted.entries.begin(); it != parted.entries.end(); ++it)
	    s << *it << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const Parted::Entry& entry)
    {
	s << "num:" << entry.num << " cylRegion:" << entry.cylRegion << " secRegion:"
	  << entry.secRegion << " type:" << toString(entry.type) << " id:" << entry.id;

	if (entry.boot)
	    s << " boot";

	return s;
    }


    void
    Parted::scanDiskFlags(const string& line)
    {
	implicit = boost::contains(line, "implicit_partition_table");
    }


    void
    Parted::scanGeometryLine(const string& line)
    {
	string tmp(line);
	tmp.erase(0, tmp.find(':') + 1);
	tmp = extractNthWord(0, tmp);

	list<string> l = splitString(extractNthWord(0, tmp), ",");

	if (l.size() == 3)
	{
	    list<string>::const_iterator i = l.begin();
	    *i++ >> geometry.cylinders;
	    *i++ >> geometry.heads;
	    *i++ >> geometry.sectors;
	}
	else
	{
	    ST_THROW( ParseException( "Bad disk geometry line", line,
				      "BIOS cylinder,head,sector geometry: 243201,255,63.  Each cylinder is 8225kB." ) );
	}
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
	    *i >> geometry.sector_size;
	}
	else
	{
	    ST_THROW( ParseException( "Bad sector size line", line,
				      "Sector size (logical/physical): 512B/4096B" ) );
	}
    }


    void
    Parted::scanCylEntryLine(const string& line)
    {
	// Sample input: (msdos disk label)
	//
	//  1      0cyl      261cyl     261cyl     primary   linux-swap(v1)  type=82
	//  2      261cyl    5484cyl    5222cyl    primary   btrfs           boot, type=83
	//  3      5484cyl   10705cyl   5221cyl    primary   btrfs           type=83
	//  4      10705cyl  243201cyl  232495cyl  extended                  lba, type=0f
	//  5      10706cyl  243200cyl  232493cyl  logical   xfs             type=83
	//
	// (Number) (Start)  (End)      (Size)     (Type)    (File system)   (Flags)
	//
	// gpt disk label: no primary/extended/logical column:
	//
	//  1      0cyl      261cyl     261cyl     linux-swap(v1)  type=82

	Entry entry;

	std::istringstream Data(line);
	classic(Data);

	unsigned long StartM = 0;
	unsigned long EndM = 0;
	unsigned long SizeM = 0;
	string PartitionTypeStr;
	string skip;

	if (label == PtType::MSDOS)
	{
	    Data >> entry.num >> StartM >> skip >> EndM >> skip >> SizeM >> skip >> PartitionTypeStr;
	}
	else
	{
	    Data >> entry.num >> StartM >> skip >> EndM >> skip >> SizeM >> skip;
	}

	if ( Data.fail() )	// parse error?
	{
	    ST_THROW( ParseException( "Bad cylinder-based partition entry", line,
+				      "2  261cyl  5484cyl  5222cyl primary  btrfs  boot, type=83" ) );
	}

	if ( entry.num == 0 )
	    ST_THROW( ParseException( "Illegal partition number 0", line, "" ) );


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

	y2mil("num:" << entry.num << " Start:" << StartM << " End:" << EndM << " Size:" << SizeM);

	unsigned long start = StartM;
	unsigned long csize = EndM-StartM+1;
	if ( start + csize > geometry.cylinders )
	{
	    csize = geometry.cylinders - start;
	    y2mil("new csize:" << csize);
	}
	entry.cylRegion = Region(start, csize);

	boost::to_lower(TInfo, locale::classic());
	list<string> flags = splitString(TInfo, ",");
	y2mil("TInfo:" << TInfo << " flags:" << flags);

	entry.boot = contains(flags, "boot");

	entry.id = ID_LINUX;

	if (label == PtType::MSDOS)
	{
	    if(  PartitionTypeStr == "extended" )
	    {
		entry.type = EXTENDED;
		entry.id = ID_EXTENDED;
	    }
	    else if ( entry.num >= 5 )
	    {
		entry.type = LOGICAL;
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

	list<string>::const_iterator it1 = find_if(flags.begin(), flags.end(),
						   string_starts_with("type="));
	if (it1 != flags.end())
	{
	    string val = string(*it1, 5);

	    if (label != PtType::MAC)
	    {
		int tmp_id = 0;
		std::istringstream Data2(val);
		classic(Data2);
		Data2 >> std::hex >> tmp_id;
		if( tmp_id>0 )
		{
		    entry.id = tmp_id;
		}
	    }
	    else
	    {
		if ( entry.id == ID_LINUX )
		{
		    if ( val.find( "apple_hfs" ) != string::npos ||
			 val.find( "apple_bootstrap" ) != string::npos )
		    {
			entry.id = ID_APPLE_HFS;
		    }
		    else if ( val.find( "apple_partition" ) != string::npos ||
			      val.find( "apple_driver" ) != string::npos ||
			      val.find( "apple_loader" ) != string::npos ||
			      val.find( "apple_boot" ) != string::npos ||
			      val.find( "apple_prodos" ) != string::npos ||
			      val.find( "apple_fwdriver" ) != string::npos ||
			      val.find( "apple_patches" ) != string::npos )
		    {
			entry.id = ID_APPLE_OTHER;
		    }
		    else if ( val.find( "apple_ufs" ) != string::npos )
		    {
			entry.id = ID_APPLE_UFS;
		    }
		}
	    }
	}

	if (label == PtType::GPT)
	{
	    if (contains(flags, "boot") && contains_if(flags, string_starts_with("fat")))
	    {
		entry.id = ID_GPT_BOOT;
	    }
	    if (contains(flags, "hp-service"))
	    {
		entry.id = ID_GPT_SERVICE;
	    }
	    if (contains(flags, "msftres"))
	    {
		entry.id = ID_GPT_MSFTRES;
	    }
	    if (contains(flags, "hfs+") || contains(flags, "hfs"))
	    {
		entry.id = ID_APPLE_HFS;
	    }
	    if (contains(flags, "bios_grub"))
	    {
		entry.id = ID_GPT_BIOS;
	    }
	    if (contains(flags, "prep"))
	    {
		entry.id = ID_GPT_PREP;
	    }
	}
	y2mil("num:" << entry.num << " id:" << entry.id << " type:" << toString(entry.type));

	entries.push_back(entry);
    }


    void
    Parted::scanSecEntryLine(const string& line)
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

	std::istringstream Data(line);
	classic(Data);

	unsigned num;
	unsigned long long startSec = 0;
	unsigned long long endSec = 0;
	unsigned long long sizeSec = 0;
	string skip;

	Data >> num >> startSec >> skip >> endSec >> skip >> sizeSec >> skip;

	if ( Data.fail() )
	{
	    ST_THROW( ParseException( "Bad sector-based partition entry", line,
				      "2  4208640s  88100863s  83892224s  primary  btrfs boot, type=83" ) );

	}

	if ( num == 0 )
	    ST_THROW( ParseException( "Illegal partition number 0", line, "" ) );


	// Search corresponding entry in 'entries' vector which was created earlier
	// from the by-cylinder output

	for (iterator it = entries.begin(); it != entries.end(); ++it)
	{
	    if (it->num == num)
	    {
		it->secRegion = Region(startSec, sizeSec);
		return;
	    }
	}

	// Entry no. 'num' not found
	ST_THROW( ParseException( "No corresponding partition number in cylinder table", line, "" ) );

    }

}
