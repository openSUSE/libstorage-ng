/*
 * Copyright (c) [2004-2014] Novell, Inc.
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

#include "storage/Utils/HumanString.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/SystemInfo/ProcMdstat.h"
#include "storage/Devices/MdImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{
    using namespace std;


    ProcMdstat::ProcMdstat()
    {
	AsciiFile mdstat(PROC_DIR "/mdstat");
	mdstat.log_content();

	parse(mdstat.get_lines());
    }


    void
    ProcMdstat::parse(const vector<string>& lines)
    {
	for (vector<string>::const_iterator it1 = lines.begin(); it1 != lines.end(); ++it1)
	{
	    if (extractNthWord(1, *it1) == ":")
	    {
		string name = extractNthWord(0, *it1);
		if (boost::starts_with(name, "md"))
		    data[name] = parse_entry(*it1, *(it1 + 1));
	    }
	}

	y2mil(*this);
    }


    ProcMdstat::Entry
    ProcMdstat::parse_entry(const string& line1, const string& line2) const
    {
	ProcMdstat::Entry entry;

	string::size_type pos;
	string tmp;

	string line = line1;
	if( (pos=line.find( ':' ))!=string::npos )
	    line.erase( 0, pos+1 );
	boost::trim_left(line, locale::classic());
	if( (pos=line.find_first_of( app_ws ))!=string::npos )
	{
	    if (line.substr(0, pos) == "active")
		line.erase(0, pos);
	}
	boost::trim_left(line, locale::classic());
	if( (pos=line.find_first_of( app_ws ))!=string::npos )
	{
	    tmp = line.substr( 0, pos );
	    if( tmp=="(read-only)" || tmp=="(auto-read-only)" || tmp=="inactive" )
	    {
		entry.read_only = true;
		entry.inactive = tmp=="inactive";
		line.erase( 0, pos );
		boost::trim_left(line, locale::classic());
	    }
	}
	boost::trim_left(line, locale::classic());
	if( (pos=line.find_first_of( app_ws ))!=string::npos )
	{
	    if( line.substr( 0, pos ).find( "active" )!=string::npos )
		line.erase( 0, pos );
	}
	boost::trim_left(line, locale::classic());

	tmp = extractNthWord( 0, line );
	if (boost::starts_with(tmp, "raid") || tmp == "linear")
	{
	    entry.md_level = toValueWithFallback(boost::to_upper_copy(tmp, locale::classic()), MdLevel::UNKNOWN);
	    if (entry.md_level == MdLevel::UNKNOWN)
		y2war("unknown raid type " << tmp);

	    if( (pos=line.find_first_of( app_ws ))!=string::npos )
		line.erase( 0, pos );
	    if( (pos=line.find_first_not_of( app_ws ))!=string::npos && pos!=0 )
		line.erase( 0, pos );
	}

	while( (pos=line.find_first_not_of( app_ws ))==0 )
	{
	    tmp = extractNthWord( 0, line );

	    string d;
	    string::size_type bracket = tmp.find( '[' );
	    if( bracket!=string::npos )
		d = normalizeDevice(tmp.substr(0, bracket));
	    else
		d = normalizeDevice(tmp);

	    // TODO can there be several of the flags?
	    bool is_spare = boost::ends_with(tmp, "(S)");
	    bool is_faulty = boost::ends_with(tmp, "(F)");
	    bool is_journal = boost::ends_with(tmp, "(J)");

	    entry.devices.emplace_back(d, is_spare, is_faulty, is_journal);

	    line.erase( 0, tmp.length() );
	    if( (pos=line.find_first_not_of( app_ws ))!=string::npos && pos!=0 )
		line.erase( 0, pos );
	}

	sort(entry.devices.begin(), entry.devices.end());

	extractNthWord(0, line2) >> entry.size;
	entry.size *= KiB;

	pos = line2.find( "chunk" );
	if( pos != string::npos )
	{
	    pos = line2.find_last_not_of( app_ws, pos-1 );
	    pos = line2.find_last_of( app_ws, pos );
	    line2.substr(pos + 1) >> entry.chunk_size;
	    entry.chunk_size *= KiB;
	}

	pos = line2.find("super");
	if (pos != string::npos)
	{
	    string::size_type pos1 = line2.find_first_of(app_ws, pos);
	    pos1 = line2.find_first_not_of(app_ws, pos1);
	    string::size_type pos2 = line2.find_first_of(app_ws, pos1);
	    entry.super = string(line2, pos1, pos2 - pos1);

	    if (entry.super == "external:ddf" || entry.super == "external:imsm")
	    {
		entry.is_container = true;
	    }
	    if (!entry.is_container && boost::starts_with(entry.super, "external:"))
	    {
		string::size_type pos1 = entry.super.find_first_of("/");
		string::size_type pos2 = entry.super.find_last_of("/");

		if (pos1 != string::npos && pos2 != string::npos && pos1 != pos2)
		{
		    entry.has_container = true;
		    entry.container_name = string(entry.super, pos1 + 1, pos2 - pos1 - 1);
		    entry.container_member = string(entry.super, pos2 + 1);
		}
	    }
	}

	entry.md_parity = MdParity::DEFAULT;
	pos = line2.find( "algorithm" );
	if( pos != string::npos )
	{
	    unsigned alg = 999;
	    pos = line2.find_first_of( app_ws, pos );
	    pos = line2.find_first_not_of( app_ws, pos );
	    line2.substr( pos ) >> alg;
	    switch( alg )
	    {
		case 0:
		    entry.md_parity = MdParity::LEFT_ASYMMETRIC;
		    break;
		case 1:
		    entry.md_parity = MdParity::RIGHT_ASYMMETRIC;
		    break;
		case 2:
		    entry.md_parity = MdParity::LEFT_SYMMETRIC;
		    break;
		case 3:
		    entry.md_parity = MdParity::RIGHT_SYMMETRIC;
		    break;
		case 4:
		    entry.md_parity = MdParity::FIRST;
		    break;
		case 5:
		    entry.md_parity = MdParity::LAST;
		    break;
		case 16:
		    entry.md_parity = MdParity::LEFT_ASYMMETRIC_6;
		    break;
		case 17:
		    entry.md_parity = MdParity::RIGHT_ASYMMETRIC_6;
		    break;
		case 18:
		    entry.md_parity = MdParity::LEFT_SYMMETRIC_6;
		    break;
		case 19:
		    entry.md_parity = MdParity::RIGHT_SYMMETRIC_6;
		    break;
		case 20:
		    entry.md_parity = MdParity::FIRST_6;
		    break;
		default:
		    y2war("unknown parity " << line2.substr(pos));
		    break;
	    }
	}
	pos = line2.find( "-copies" );
	if( pos != string::npos )
	{
	    unsigned num = 0;
	    string where;
	    pos = line2.find_last_of( app_ws, pos );
	    line2.substr( pos ) >> where;
	    pos = line2.find_last_not_of( app_ws, pos );
	    pos = line2.find_last_of( app_ws, pos );
	    line2.substr( pos ) >> num;
	    y2mil( "where:" << where << " num:" << num );
	    if (where == "near-copies")
		entry.md_parity = (num == 3) ? MdParity::NEAR_3 : MdParity::NEAR_2;
	    else if (where == "far-copies")
		entry.md_parity = (num == 3)? MdParity::FAR_3 : MdParity::FAR_2;
	    else if (where == "offset-copies")
		entry.md_parity = (num == 3) ? MdParity::OFFSET_3 : MdParity::OFFSET_2;
	}

	return entry;
    }


    vector<string>
    ProcMdstat::get_entries() const
    {
	vector<string> ret;
	for (const_iterator i = data.begin(); i != data.end(); ++i)
	    ret.push_back(i->first);
	return ret;
    }


    bool
    ProcMdstat::has_entry(const string& name) const
    {
	return data.find(name) != data.end();
    }


    const ProcMdstat::Entry&
    ProcMdstat::get_entry(const string& name) const
    {
	const_iterator it = data.find(name);
	if (it == data.end())
	    ST_THROW(Exception("entry not found"));

	return it->second;
    }


    std::ostream&
    operator<<(std::ostream& s, const ProcMdstat& proc_mdstat)
    {
	for (const pair<const string, ProcMdstat::Entry>& entry : proc_mdstat.data)
	    s << "data[" << entry.first << "] -> " << entry.second << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const ProcMdstat::Entry& entry)
    {
	s << "md-level:" << toString(entry.md_level);

	if (entry.md_parity != MdParity::DEFAULT)
	    s << " md-parity:" + toString(entry.md_parity);

	if (!entry.super.empty())
	    s << " super:" + entry.super;

	if (entry.chunk_size != 0)
	    s << " chunk-size:" << entry.chunk_size;

	s << " size:" << entry.size;

	if (entry.read_only)
	    s << " read-only";

	if (entry.inactive)
	    s << " inactive";

	s << " devices:" << entry.devices;

	if (entry.is_container)
	    s << " is-container";

	if (entry.has_container)
	    s << " has-container container-name:" << entry.container_name << " container-member:"
	      << entry.container_member;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const ProcMdstat::Device& device)
    {
	s << device.name;

	if (device.spare)
	    s << "(S)";
	if (device.faulty)
	    s << "(F)";
	if (device.journal)
	    s << "(J)";

	return s;
    }

}
