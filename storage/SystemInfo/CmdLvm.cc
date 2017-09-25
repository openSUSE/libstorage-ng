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

#include "storage/Devices/LvmLvImpl.h"
#include "storage/SystemInfo/CmdLvm.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/JsonFile.h"


#define COMMON_LVM_OPTIONS "--reportformat json --units b --nosuffix"


namespace storage
{
    using namespace std;


    void
    CmdLvm::parse(const vector<string>& lines, const char* tag)
    {
	JsonFile json_file(lines);

	vector<json_object*> tmp1;
	if (get_child_nodes(json_file.get_root(), "report", tmp1))
	{
	    for (json_object* tmp2 : tmp1)
	    {
		vector<json_object*> tmp3;
		if (get_child_nodes(tmp2, tag, tmp3))
		{
		    for (json_object* tmp4 : tmp3)
			parse(tmp4);
		}
	    }
	}
    }


    CmdPvs::CmdPvs()
    {
	SystemCmd cmd(PVSBIN " " COMMON_LVM_OPTIONS " --options pv_name,pv_uuid,vg_name,vg_uuid,"
		      "pv_attr");
	if (cmd.retcode() == 0 && !cmd.stdout().empty())
	    parse(cmd.stdout());
    }


    void
    CmdPvs::parse(const vector<string>& lines)
    {
	pvs.clear();

	CmdLvm::parse(lines, "pv");

	sort(pvs.begin(), pvs.end(), [](const Pv& lhs, const Pv& rhs) { return lhs.pv_name < rhs.pv_name; });

	y2mil(*this);
    }


    void
    CmdPvs::parse(json_object* object)
    {
	Pv pv;

	get_child_value(object, "pv_name", pv.pv_name);
	get_child_value(object, "pv_uuid", pv.pv_uuid);

	get_child_value(object, "vg_name", pv.vg_name);
	get_child_value(object, "vg_uuid", pv.vg_uuid);

	string pv_attr;
	get_child_value(object, "pv_attr", pv_attr);
	if (pv_attr.size() < 3)
	    ST_THROW(ParseException("bad pv_attr", pv_attr, "a--"));

	pvs.push_back(pv);
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
	for (const CmdPvs::Pv& pv : cmd_pvs.pvs)
	    s << "pv:{ " << pv << " }\n";

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
	SystemCmd cmd(LVSBIN " " COMMON_LVM_OPTIONS " --options lv_name,lv_uuid,vg_name,"
		      "vg_uuid,lv_role,lv_attr,lv_size,pool_lv,pool_lv_uuid");

	if (cmd.retcode() == 0 && !cmd.stdout().empty())
	    parse(cmd.stdout());
    }


    void
    CmdLvs::parse(const vector<string>& lines)
    {
	lvs.clear();

	CmdLvm::parse(lines, "lv");

	sort(lvs.begin(), lvs.end(), [](const Lv& lhs, const Lv& rhs) { return lhs.lv_name < rhs.lv_name; });

	y2mil(*this);
    }


    void
    CmdLvs::parse(json_object* object)
    {
	Lv lv;

	get_child_value(object, "lv_name", lv.lv_name);
	get_child_value(object, "lv_uuid", lv.lv_uuid);

	get_child_value(object, "lv_size", lv.size);

	get_child_value(object, "vg_name", lv.vg_name);
	get_child_value(object, "vg_uuid", lv.vg_uuid);

	string lv_attr;
	get_child_value(object, "lv_attr", lv_attr);

	lv.active = lv_attr[4] == 'a';

	switch (lv_attr[0])
	{
	    case 't': lv.lv_type = LvType::THIN_POOL; break;
	    case 'V': lv.lv_type = LvType::THIN; break;
	}

	get_child_value(object, "pool_lv", lv.pool_name);
	get_child_value(object, "pool_lv_uuid", lv.pool_uuid);

	lvs.push_back(lv);
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
	for (const CmdLvs::Lv& lv : cmd_lvs.lvs)
	    s << "lv:{ " << lv << " }\n";

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLvs::Lv& lv)
    {
	s << "lv-name:" << lv.lv_name << " lv-uuid:" << lv.lv_uuid << " vg-name:"
	  << lv.vg_name << " vg-uuid:" << lv.vg_uuid << " lv-type:" << toString(lv.lv_type)
	  << " active:" << lv.active << " size:" << lv.size;

	if (!lv.pool_name.empty())
	    s << " pool-name:" << lv.pool_name;

	if (!lv.pool_uuid.empty())
	    s << " pool-uuid:" << lv.pool_uuid;

	return s;
    }


    CmdVgs::CmdVgs()
    {
	SystemCmd cmd(VGSBIN " " COMMON_LVM_OPTIONS " --options vg_name,vg_uuid,vg_attr,"
		      "vg_extent_size,vg_extent_count,vg_free_count");
	if (cmd.retcode() == 0 && !cmd.stdout().empty())
	    parse(cmd.stdout());
    }


    void
    CmdVgs::parse(const vector<string>& lines)
    {
	vgs.clear();

	CmdLvm::parse(lines, "vg");

	sort(vgs.begin(), vgs.end(), [](const Vg& lhs, const Vg& rhs) { return lhs.vg_name < rhs.vg_name; });

	y2mil(*this);
    }


    void
    CmdVgs::parse(json_object* object)
    {
	Vg vg;

	get_child_value(object, "vg_name", vg.vg_name);
	get_child_value(object, "vg_uuid", vg.vg_uuid);

	string vg_attr;
	get_child_value(object, "vg_attr", vg_attr);
	if (vg_attr.size() < 6)
	    ST_THROW(ParseException("bad vg_attr", vg_attr, "wz--n-"));

	get_child_value(object, "vg_extent_size", vg.extent_size);
	get_child_value(object, "vg_extent_count", vg.extent_count);
	get_child_value(object, "vg_free_count", vg.free_extent_count);

	vgs.push_back(vg);
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
	for (const CmdVgs::Vg& vg : cmd_vgs.vgs)
	    s << "vg:{ " << vg << " }\n";

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdVgs::Vg& vg)
    {
	s << "vg-name:" << vg.vg_name << " vg-uuid:" << vg.vg_uuid << " extent-size:"
	  << vg.extent_size << " extent-count:" << vg.extent_count << " free-extent-count:"
	  << vg.free_extent_count;

	return s;
    }

}
