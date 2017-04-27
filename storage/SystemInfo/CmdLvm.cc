/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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
#include <vector>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdLvm.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


#define COMMON_LVM_OPTIONS "--noheadings --unbuffered --units b --nosuffix"

// TODO using --nameprefixes should make parsers robust to strange characters


namespace storage
{
    using namespace std;


    CmdPvs::CmdPvs()
    {
	SystemCmd c(PVSBIN " " COMMON_LVM_OPTIONS " --options pv_name,pv_uuid,vg_name,vg_uuid,"
		    "pv_attr");
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

	    string attr;

	    data >> pv.pv_name >> pv.pv_uuid >> pv.vg_name >> pv.vg_uuid >> attr;

	    if (attr.size() < 3)
		ST_THROW(ParseException("bad pv_attr", attr, "a--"));

	    pvs.push_back(pv);
	}

	sort(pvs.begin(), pvs.end(), [](const Pv& lhs, const Pv& rhs) { return lhs.pv_name < rhs.pv_name; });

	y2mil(*this);
    }


    const CmdPvs::Pv&
    CmdPvs::find_by_pv_uuid(const string& pv_uuid) const
    {
	for (const Pv& pv : pvs)
	{
	    if (pv.pv_uuid == pv_uuid)
		return pv;
	}

	ST_THROW(Exception("pv not found by pv-uuid"));
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
	SystemCmd c(LVSBIN " " COMMON_LVM_OPTIONS " --options lv_name,lv_uuid,vg_name,vg_uuid,"
		    "lv_attr,lv_size");
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

	    string attr;
	    data >> lv.lv_name >> lv.lv_uuid >> lv.vg_name >> lv.vg_uuid >> attr >> lv.size;

	    if (attr.size() < 10)
		ST_THROW(ParseException("bad lv_attr", attr, "-wi-ao----"));

	    lv.active = attr[4] == 'a';

	    lvs.push_back(lv);
	}

	sort(lvs.begin(), lvs.end(), [](const Lv& lhs, const Lv& rhs) { return lhs.lv_name < rhs.lv_name; });

	y2mil(*this);
    }


    const CmdLvs::Lv&
    CmdLvs::find_by_lv_uuid(const string& lv_uuid) const
    {
	for (const Lv& lv : lvs)
	{
	    if (lv.lv_uuid == lv_uuid)
		return lv;
	}

	ST_THROW(Exception("lv not found by lv-uuid"));
    }


    size_t
    CmdLvs::number_of_inactive() const
    {
	return count_if(lvs.begin(), lvs.end(), [](const Lv& lv) { return !lv.active; });
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
	  << lv.vg_name << " vg-uuid:" << lv.vg_uuid << " active:" << lv.active
	  << " size:" << lv.size;

	return s;
    }


    CmdVgs::CmdVgs()
    {
	SystemCmd c(VGSBIN " " COMMON_LVM_OPTIONS " --options vg_name,vg_uuid,vg_attr,"
		    "vg_extent_size,vg_extent_count");
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

	    string attr;

	    data >> vg.vg_name >> vg.vg_uuid >> attr >> vg.extent_size >> vg.extent_count;

	    if (attr.size() < 6)
		ST_THROW(ParseException("bad vg_attr", attr, "wz--n-"));

	    vgs.push_back(vg);
	}

	sort(vgs.begin(), vgs.end(), [](const Vg& lhs, const Vg& rhs) { return lhs.vg_name < rhs.vg_name; });

	y2mil(*this);
    }


    const CmdVgs::Vg&
    CmdVgs::find_by_vg_uuid(const string& vg_uuid) const
    {
	for (const Vg& vg : vgs)
	{
	    if (vg.vg_uuid == vg_uuid)
		return vg;
	}

	ST_THROW(Exception("vg not found by vg-uuid"));
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
	s << "vg-name:" << vg.vg_name << " vg-uuid:" << vg.vg_uuid << " extent-size:"
	  << vg.extent_size << " extent-count:" << vg.extent_count;

	return s;
    }

}
