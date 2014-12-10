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


#include <fstream>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/StorageDefines.h"
#include "storage/SystemInfo/CmdParted.h"
#include "storage/Utils/Enum.h"
#include "storage/Devices/Partition.h"
#include "storage/StorageTypes.h"


namespace storage_bgl
{
    using namespace std;


    Parted::Parted(const string& device, bool do_probe)
	: device(device), label(PtType::PT_UNKNOWN), implicit(false), gpt_enlarge(false)
    {
	if (do_probe)
	    probe();
    }


    void
    Parted::probe()
    {
	SystemCmd cmd(PARTEDCMD + quote(device) + " unit cyl print unit s print");

	// No check for exit status since parted 3.1 exits with 1 if no
	// partition table is found.

	parse(cmd.stdout());
    }


    void
    Parted::parse(const vector<string>& lines)
    {
	implicit = false;
	gpt_enlarge = false;
	entries.clear();

	vector<string>::const_iterator pos;

	pos = find_if(lines, string_starts_with("Partition Table:"));
	if (pos != lines.end())
	{
	    string label_str = extractNthWord(2, *pos);
	    if (label_str == "msdos")
		label = PtType::MSDOS;
	    else if (label_str == "gpt" || label_str == "gpt_sync_mbr")
		label = PtType::GPT;
	    else if (label_str == "dasd")
		label = PtType::DASD;
	    else if (label_str == "loop")
		label = PtType::PT_UNKNOWN;
	    else
		throw runtime_error("unknown partition table type");
	}
	else
	    y2war("could not find partition table");

	// only present for unrecognised disk label due to patch in parted
	pos = find_if(lines, string_starts_with("BIOS cylinder,head,sector geometry:"));
	if (pos != lines.end())
	    scanGeometryLine(*pos);
	else
	    y2err("could not find geometry");

	// see bnc #866535
	pos = find_if(lines, string_starts_with("Disk " + device + ":"));
	if (pos != lines.end())
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
	pos = find_if(lines, string_starts_with("Sector size (logical/physical):"));
	if (pos != lines.end())
	    scanSectorSizeLine(*pos);
	else
	    y2war("could not find sector size");

	pos = find_if(lines, string_starts_with("Disk Flags:"));
	if (pos != lines.end())
	    scanDiskFlags(*pos);
	else
	    y2war("could not find disk flags");

	gpt_enlarge = find_if(lines, string_starts_with("fix the GPT to use all")) != lines.end();

	if (label != PtType::PT_UNKNOWN)
	{
	    int n = 0;

	    for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	    {
		if (boost::starts_with(*it, "Number"))
		    n++;

		string tmp = extractNthWord(0, *it);
		if (!tmp.empty() && isdigit(tmp[0]))
		{
		    assert(n == 1 || n == 2);
		    if (n == 1)
			scanCylEntryLine(*it);
		    else if (n == 2)
			scanSecEntryLine(*it);
		}
	    }

	    iterator prev = entries.begin();
	    iterator it = prev;
	    if (it != entries.end())
		++it;
	    while (it != entries.end())
	    {
		if (prev->cylRegion.start() >= it->cylRegion.start() && it->cylRegion.len() > 0)
		{
		    y2mil("old:" << it->cylRegion);
		    it->cylRegion.setStart(it->cylRegion.start() + 1);
		    it->cylRegion.setLen(it->cylRegion.len() - 1);
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
	assert(l.size() == 3);
	if (l.size() == 3)
	{
	    list<string>::const_iterator i = l.begin();
	    *i++ >> geometry.cylinders;
	    *i++ >> geometry.heads;
	    *i++ >> geometry.sectors;
	}
	else
	{
	    y2err("could not find geometry");
	}
    }


    void
    Parted::scanSectorSizeLine(const string& line)
    {
	string tmp(line);
	tmp.erase(0, tmp.find(':') + 1);
	tmp = extractNthWord(0, tmp);

	list<string> l = splitString(extractNthWord(0, tmp), "/");
	assert(l.size() == 2);
	if (l.size() == 2)
	{
	    list<string>::const_iterator i = l.begin();
	    *i >> geometry.sector_size;
	}
	else
	{
	    y2war("could not find sector size");
	}
    }


    void
    Parted::scanCylEntryLine(const string& line)
    {
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

	assert(!Data.fail());
	assert(entry.num != 0);

	if (Data.fail() || entry.num == 0)
	{
	    y2err("invalid line:" << line);
	    return;
	}

	char c;
	string TInfo;
	Data.unsetf(ifstream::skipws);
	Data >> c;
	char last_char = ',';
	while( Data.good() && !Data.eof() )
	{
	    if( !isspace(c) )
	    {
		TInfo += c;
		last_char = c;
	    }
	    else
	    {
		if( last_char != ',' )
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
	if( start + csize > geometry.cylinders )
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
	    if( PartitionTypeStr == "extended" )
	    {
		entry.type = storage::EXTENDED;
		entry.id = ID_EXTENDED;
	    }
	    else if( entry.num >= 5 )
	    {
		entry.type = storage::LOGICAL;
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
		if( entry.id == ID_LINUX )
		{
		    if( val.find( "apple_hfs" ) != string::npos ||
			val.find( "apple_bootstrap" ) != string::npos )
		    {
			entry.id = ID_APPLE_HFS;
		    }
		    else if( val.find( "apple_partition" ) != string::npos ||
			     val.find( "apple_driver" ) != string::npos ||
			     val.find( "apple_loader" ) != string::npos ||
			     val.find( "apple_boot" ) != string::npos ||
			     val.find( "apple_prodos" ) != string::npos ||
			     val.find( "apple_fwdriver" ) != string::npos ||
			     val.find( "apple_patches" ) != string::npos )
		    {
			entry.id = ID_APPLE_OTHER;
		    }
		    else if( val.find( "apple_ufs" ) != string::npos )
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
	std::istringstream Data(line);
	classic(Data);

	unsigned num;
	unsigned long long startSec = 0;
	unsigned long long endSec = 0;
	unsigned long long sizeSec = 0;
	string skip;

	Data >> num >> startSec >> skip >> endSec >> skip >> sizeSec >> skip;

	assert(!Data.fail());
	assert(num != 0);

	if (Data.fail() || num == 0)
	{
	    y2err("invalid line:" << line);
	    return;
	}

	for (iterator it = entries.begin(); it != entries.end(); ++it)
	{
	    if (it->num == num)
	    {
		it->secRegion = Region(startSec, sizeSec);
		return;
	    }
	}
    }

}
