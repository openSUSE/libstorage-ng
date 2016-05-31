/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#include <string>
#include <list>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdLvm.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{
    using namespace std;


    CmdPvs::CmdPvs()
    {
	SystemCmd c(PVSBIN " --noheadings --unbuffered --options pv_name,pv_uuid,vg_name,vg_uuid");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdPvs::parse(const vector<string>& lines)
    {
	pvs.clear();

	for (const string& line : lines)
	{
	    std::istringstream data(line);
	    classic(data);

	    Pv pv;
	    data >> pv.pv_name >> pv.pv_uuid >> pv.vg_name >> pv.vg_uuid;
	    pvs.push_back(pv);
	}

	y2mil(*this);
    }


    const CmdPvs::Pv&
    CmdPvs::get_pv_by_pv_uuid(const string& pv_uuid) const
    {
	for (const Pv& pv : pvs)
	{
	    if (pv.pv_uuid == pv_uuid)
		return pv;
	}

	ST_THROW(Exception("pv not found by pv-uuid"));
	__builtin_unreachable();
    }


    vector<CmdPvs::Pv>
    CmdPvs::get_pvs_by_vg_uuid(const string& vg_uuid) const
    {
	vector<Pv> ret;

	for (const Pv& pv : pvs)
	{
	    if (pv.vg_uuid == vg_uuid)
		ret.push_back(pv);
	}

	return ret;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdPvs& cmd_pvs)
    {
	s << "pvs:" << cmd_pvs.pvs;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdPvs::Pv& pv)
    {
	s << "pv-name:" << pv.pv_name << " pv-uuid:" << pv.pv_uuid << " vg-name:"
	  << pv.vg_name << " vg-uuid:" << pv.vg_uuid;

	return s;
    }


    CmdLvs::CmdLvs()
    {
	SystemCmd c(LVSBIN " --noheadings --unbuffered --options lv_name,lv_uuid,vg_name,vg_uuid");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdLvs::parse(const vector<string>& lines)
    {
	lvs.clear();

	for (const string& line : lines)
	{
	    std::istringstream data(line);
	    classic(data);

	    Lv lv;
	    data >> lv.lv_name >> lv.lv_uuid >> lv.vg_name >> lv.vg_uuid;
	    lvs.push_back(lv);
	}

	y2mil(*this);
    }


    const CmdLvs::Lv&
    CmdLvs::get_lv_by_lv_uuid(const string& lv_uuid) const
    {
	for (const Lv& lv : lvs)
	{
	    if (lv.lv_uuid == lv_uuid)
		return lv;
	}

	ST_THROW(Exception("lv not found by lv-uuid"));
	__builtin_unreachable();
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLvs& cmd_lvs)
    {
	s << "lvs:" << cmd_lvs.lvs;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLvs::Lv& lv)
    {
	s << "lv-name:" << lv.lv_name << " lv-uuid:" << lv.lv_uuid << " vg-name:"
	  << lv.vg_name << " vg-uuid:" << lv.vg_uuid;

	return s;
    }


    CmdVgs::CmdVgs()
    {
	SystemCmd c(VGSBIN " --noheadings --unbuffered --options vg_name,vg_uuid");
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdVgs::parse(const vector<string>& lines)
    {
	vgs.clear();

	for (const string& line : lines)
	{
	    std::istringstream data(line);
	    classic(data);

	    Vg vg;
	    data >> vg.vg_name >> vg.vg_uuid;
	    vgs.push_back(vg);
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdVgs& cmd_vgs)
    {
	s << "vgs:" << cmd_vgs.vgs;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdVgs::Vg& vg)
    {
	s << "vg-name:" << vg.vg_name << " vg-uuid:" << vg.vg_uuid;

	return s;
    }


    CmdVgdisplay::CmdVgdisplay(const string& name)
	: name(name), pe_size(0), num_pe(0), free_pe(0), read_only(false), lvm1(false)
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


    std::ostream&
    operator<<(std::ostream& s, const CmdVgdisplay::LvEntry& lv_entry)
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


    std::ostream&
    operator<<(std::ostream& s, const CmdVgdisplay::PvEntry& pv_entry)
    {
	s << "device:" << pv_entry.device << " uuid:" << pv_entry.uuid
	  << " status:" << pv_entry.status << " num_pe:" << pv_entry.num_pe
	  << " free_pe:" << pv_entry.free_pe;

	return s;
    }

}
