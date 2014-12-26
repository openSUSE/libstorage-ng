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


#include <string>
#include <list>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdLvm.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{
    using namespace std;


    CmdVgs::CmdVgs(bool do_probe)
    {
	if (do_probe)
	    probe();
    }


    void
    CmdVgs::probe()
    {
	SystemCmd c(VGSBIN " --noheadings --unbuffered --options vg_name");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdVgs::parse(const vector<string>& lines)
    {
	vgs.clear();

	for (const string& line : lines)
	    vgs.push_back(boost::trim_copy(line, locale::classic()));

	y2mil(*this);
    }


    std::ostream& operator<<(std::ostream& s, const CmdVgs& cmdvgs)
    {
	s << "vgs:" << cmdvgs.vgs;

	return s;
    }


    CmdVgdisplay::CmdVgdisplay(const string& name, bool do_probe)
	: name(name), pe_size(0), num_pe(0), free_pe(0), read_only(false), lvm1(false)
    {
	if (do_probe)
	    probe();
    }


    void
    CmdVgdisplay::probe()
    {
	SystemCmd c(VGDISPLAYBIN " --units k --verbose " + quote(name));
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdVgdisplay::parse(const vector<string>& lines)
    {
	vector<string>::const_iterator it = lines.begin();

	while (it != lines.end())
	{
	    string line = *it++;
	    if (boost::contains(line, "Volume group"))
	    {
		line = *it++;
		while (!boost::contains(line, "Logical volume") &&
		       !boost::contains(line, "Physical volume") &&
		       it != lines.end())
		{
		    line.erase(0, line.find_first_not_of(app_ws));
		    if (boost::starts_with(line, "Format"))
		    {
			lvm1 = extractNthWord(1, line) == "lvm1";
		    }
		    else if (boost::starts_with(line, "PE Size"))
		    {
			string tmp = extractNthWord(2, line);
			string::size_type pos = tmp.find('.');
			if (pos!=string::npos)
			    tmp.erase(pos);
			tmp >> pe_size;
		    }
		    else if (boost::starts_with(line, "Total PE"))
		    {
			extractNthWord(2, line) >> num_pe;
		    }
		    else if (boost::starts_with(line, "Free  PE"))
		    {
			extractNthWord(4, line) >> free_pe;
		    }
		    else if (boost::starts_with(line, "VG Status"))
		    {
			status = extractNthWord(2, line);
		    }
		    else if (boost::starts_with(line, "VG Access"))
		    {
			read_only = !boost::contains(extractNthWord(2, line, true), "write");
		    }
		    else if (boost::starts_with(line, "VG UUID"))
		    {
			uuid = extractNthWord(2, line);
		    }
		    line = *it++;
		}

		LvEntry lv_entry;
		lv_entry.clear();
		while (!boost::contains(line, "Physical volume") &&
		       it != lines.end())
		{
		    line.erase(0, line.find_first_not_of(app_ws));
		    if (boost::starts_with(line, "LV Name"))
		    {
			if (!lv_entry.name.empty())
			    lv_entries.push_back(lv_entry);
			lv_entry.clear();
			lv_entry.name = extractNthWord(2, line);
			string::size_type pos = lv_entry.name.rfind("/");
			if (pos != string::npos)
			    lv_entry.name.erase(0, pos + 1);
		    }
		    else if (boost::starts_with(line, "LV Write Access"))
		    {
			lv_entry.read_only = boost::contains(extractNthWord(3, line, true), "only");
		    }
		    else if (boost::starts_with(line, "LV snapshot status"))
		    {
			if (boost::contains(line, "destination for"))
			{
			    lv_entry.origin = extractNthWord(6, line, true);
			    string::size_type pos = lv_entry.origin.find("/", 5);
			    if (pos != string::npos)
				lv_entry.origin.erase(0, pos + 1);
			}
		    }
		    else if (boost::starts_with(line, "LV Status"))
		    {
			lv_entry.status = extractNthWord(2, line, true);
		    }
		    else if (boost::starts_with(line, "Current LE"))
		    {
			extractNthWord(2, line) >> lv_entry.num_le;
		    }
		    else if (boost::starts_with(line, "COW-table LE"))
		    {
			extractNthWord(2, line) >> lv_entry.num_cow_le;
		    }
		    else if (boost::starts_with(line, "LV UUID"))
		    {
			lv_entry.uuid = extractNthWord(2, line);
		    }
		    else if (boost::starts_with(line, "LV Pool metadata"))
		    {
			lv_entry.pool = true;
		    }
		    else if (boost::starts_with(line, "LV Pool chunk size"))
                    {
			extractNthWord(4, line) >> lv_entry.pool_chunk;
                    }
		    else if (boost::starts_with(line, "LV Pool name"))
                    {
			lv_entry.used_pool = extractNthWord(3, line);
                    }
		    line = *it++;
		}
		if (!lv_entry.name.empty())
		    lv_entries.push_back(lv_entry);

		PvEntry pv_entry;
		pv_entry.clear();
		while (it != lines.end())
		{
		    line.erase(0, line.find_first_not_of(app_ws));
		    if (boost::starts_with(line, "PV Name"))
		    {
			if (!pv_entry.device.empty())
			    pv_entries.push_back(pv_entry);
			pv_entry.clear();
			pv_entry.device = extractNthWord(2, line);
		    }
		    else if (boost::starts_with(line, "PV UUID"))
		    {
			pv_entry.uuid = extractNthWord(2, line);
		    }
		    else if (boost::starts_with(line, "PV Status"))
		    {
			pv_entry.status = extractNthWord(2, line);
		    }
		    else if (boost::starts_with(line, "Total PE"))
		    {
			extractNthWord(5, line) >> pv_entry.num_pe;
			extractNthWord(7, line) >> pv_entry.free_pe;
		    }
		    line = *it++;
		}
		if (!pv_entry.device.empty())
		    pv_entries.push_back(pv_entry);
	    }
	}

	y2mil(*this);
    }


    void
    CmdVgdisplay::LvEntry::clear()
    {
	name.clear();
	uuid.clear();
	status.clear();
	origin.clear();
	used_pool.clear();
	num_le = 0;
	num_cow_le = 0;
	pool_chunk = 0;
	read_only = false;
	pool = false;
    }


    void
    CmdVgdisplay::PvEntry::clear()
    {
	 device.clear();
	 uuid.clear();
	 status.clear();
	 num_pe = 0;
	 free_pe = 0;
    }


    std::ostream& operator<<(std::ostream& s, const CmdVgdisplay& cmdvgdisplay)
    {
	s << "name:" << cmdvgdisplay.name << " uuid:" << cmdvgdisplay.uuid
	  << " status:" << cmdvgdisplay.status << " pe_size:" << cmdvgdisplay.pe_size
	  << " num_pe:" << cmdvgdisplay.num_pe << " free_pe:" << cmdvgdisplay.free_pe;

	if (cmdvgdisplay.read_only)
	    s << " read_only";

	if (cmdvgdisplay.lvm1)
	    s << " lvm1";

	s << endl;

	for (const CmdVgdisplay::LvEntry& lv_entry : cmdvgdisplay.lv_entries)
	    s << "lv " << lv_entry << endl;

	for (const CmdVgdisplay::PvEntry& pv_entry : cmdvgdisplay.pv_entries)
	    s << "pv " << pv_entry << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const CmdVgdisplay::LvEntry& lv_entry)
    {
	s << "name:" << lv_entry.name << " uuid:" << lv_entry.uuid
	  << " status:" << lv_entry.status;

	if (!lv_entry.origin.empty())
	    s << " origin:" << lv_entry.origin;

	if (!lv_entry.used_pool.empty())
	    s << " used_pool:" << lv_entry.used_pool;

	s << " num_le:" << lv_entry.num_le;

	if (lv_entry.num_cow_le != 0)
	    s << " num_cow_le:" << lv_entry.num_cow_le;

	if (lv_entry.pool_chunk != 0)
	    s << " pool_chunk:" << lv_entry.pool_chunk;

	if (lv_entry.read_only)
	    s << " read_only";

	if (lv_entry.pool)
	    s << " pool";

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const CmdVgdisplay::PvEntry& pv_entry)
    {
	s << "device:" << pv_entry.device << " uuid:" << pv_entry.uuid
	  << " status:" << pv_entry.status << " num_pe:" << pv_entry.num_pe
	  << " free_pe:" << pv_entry.free_pe;

	return s;
    }

}
