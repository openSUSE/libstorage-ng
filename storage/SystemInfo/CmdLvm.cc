/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2025] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdLvm.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


// see bsc #1186780
#define CONFIG_OVERRIDE "--config", "log { command_names = 0 prefix = \"\" }"

#define COMMON_LVM_OPTIONS "--reportformat", "json", CONFIG_OVERRIDE, "--units", "b", "--nosuffix"


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


#define PVS_OPTIONS "pv_name,pv_uuid,vg_name,vg_uuid,pv_attr,pe_start"


    CmdPvs::CmdPvs()
    {
	SystemCmd cmd({ PVS_BIN, COMMON_LVM_OPTIONS, "--all", "--options",  PVS_OPTIONS }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    CmdPvs::CmdPvs(const string& pv_name)
    {
	SystemCmd cmd({ PVS_BIN, COMMON_LVM_OPTIONS, "--all", "--options", PVS_OPTIONS, pv_name },
		      SystemCmd::DoThrow);

	parse(cmd.stdout());

	if (pvs.size() != 1)
	    ST_THROW(Exception("command pvs returned wrong number of pvs"));
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

	if (pv.pv_uuid.empty())
	    return;

	get_child_value(object, "vg_name", pv.vg_name);
	get_child_value(object, "vg_uuid", pv.vg_uuid);

	string pv_attr;
	get_child_value(object, "pv_attr", pv_attr);
	if (pv_attr.size() < 3)
	    ST_THROW(ParseException("bad pv_attr", pv_attr, "a--"));

	pv.missing = pv_attr[2] == 'm';
	pv.duplicate = pv_attr[0] == 'd';

	get_child_value(object, "pe_start", pv.pe_start);

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
	s << "pv-name:" << pv.pv_name << " pv-uuid:" << pv.pv_uuid << " vg-name:" << pv.vg_name
	  << " vg-uuid:" << pv.vg_uuid << " pe-start:" << pv.pe_start;

	if (pv.missing)
	    s << " missing";

	if (pv.duplicate)
	    s << " duplicate";

	return s;
    }


#define LVS_OPTIONS "lv_name,lv_uuid,vg_name,vg_uuid,lv_role,lv_attr,lv_size,origin_size,segtype,"	\
	"stripes,stripe_size,chunk_size,pool_lv,pool_lv_uuid,origin,origin_uuid,data_lv,data_lv_uuid,"	\
	"metadata_lv,metadata_lv_uuid"


    CmdLvs::CmdLvs()
    {
	// Note: Querying segtype, origin, origin_uuid and origin_size is rather new and
	// not available in all testsuite data.

	SystemCmd cmd({ LVS_BIN, COMMON_LVM_OPTIONS, "--all", "--options", LVS_OPTIONS }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    CmdLvs::CmdLvs(const string& vg_name, const string& lv_name)
    {
	SystemCmd cmd({ LVS_BIN, COMMON_LVM_OPTIONS, "--all", "--options", LVS_OPTIONS, "--",
		vg_name + "/" + lv_name }, SystemCmd::DoThrow);

	parse(cmd.stdout());

	if (lvs.size() != 1)
	    ST_THROW(Exception("command lvs returned wrong number of lvs"));

	const Lv& lv = lvs[0];

	if (lv.vg_name != vg_name)
	    ST_THROW(Exception("command lvs returned wrong vg_name"));

	if (lv.lv_name != lv_name)
	    ST_THROW(Exception("command lvs returned wrong lv_name"));
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
	Segment segment;

	get_child_value(object, "lv_name", lv.lv_name);
	get_child_value(object, "lv_uuid", lv.lv_uuid);

	get_child_value(object, "lv_size", lv.size);
	get_child_value(object, "origin_size", lv.origin_size);

	get_child_value(object, "vg_name", lv.vg_name);
	get_child_value(object, "vg_uuid", lv.vg_uuid);

	string lv_role;
	get_child_value(object, "lv_role", lv_role);
	lv.role = parse_role(lv_role);

	string lv_attr;
	get_child_value(object, "lv_attr", lv_attr);
	if (lv_attr.size() < 10)
	    ST_THROW(ParseException("bad lv_attr", lv_attr, "-wi-ao----"));

	lv.active = lv_attr[4] == 'a';

	switch (lv_attr[0])
	{
	    case '-': lv.lv_type = LvType::NORMAL; break;
	    case 't': lv.lv_type = LvType::THIN_POOL; break;
	    case 'V': lv.lv_type = LvType::THIN; break;
	    case 'r': lv.lv_type = LvType::RAID; break;
	    case 's': lv.lv_type = LvType::SNAPSHOT; break;
	    case 'm': lv.lv_type = LvType::MIRROR; break;

	    case 'o':
	    case 'C':
	    {
		string segtype;
		get_child_value(object, "segtype", segtype);
		if (segtype.empty())
		    ST_THROW(ParseException("bad segtype", segtype, "linear"));

		if (segtype == "cache-pool")
		    lv.lv_type = LvType::CACHE_POOL;
		else if (segtype == "cache")
		    lv.lv_type = LvType::CACHE;
		else if (segtype == "writecache")
		    lv.lv_type = LvType::WRITECACHE;
		else if (segtype == "mirror")
		    lv.lv_type = LvType::MIRROR;
		else if (boost::starts_with(segtype, "raid"))
		    lv.lv_type = LvType::RAID;
		else if (segtype == "linear" || segtype == "striped")
		    lv.lv_type = LvType::NORMAL;
	    }
	    break;
	}

	get_child_value(object, "pool_lv", lv.pool_name);
	get_child_value(object, "pool_lv_uuid", lv.pool_uuid);

	get_child_value(object, "origin", lv.origin_name);
	get_child_value(object, "origin_uuid", lv.origin_uuid);

	get_child_value(object, "data_lv", lv.data_name);
	get_child_value(object, "data_lv_uuid", lv.data_uuid);

	get_child_value(object, "metadata_lv", lv.metadata_name);
	get_child_value(object, "metadata_lv_uuid", lv.metadata_uuid);

	get_child_value(object, "stripes", segment.stripes);
	get_child_value(object, "stripe_size", segment.stripe_size);

	get_child_value(object, "chunk_size", segment.chunk_size);

	// The stripes and chunksize options makes lvs print every segment of
	// a LV. Depending on whether the LV is already in lvs, either add the
	// complete LV or only the segment to the already existing LV.

	vector<Lv>::iterator it = find_if(lvs.begin(), lvs.end(), [lv](const Lv& tmp) {
	    return lv.lv_uuid == tmp.lv_uuid;
	});

	if (it == lvs.end())
	{
	    lv.segments.push_back(segment);
	    lvs.push_back(lv);
	}
	else
	{
	    it->segments.push_back(segment);
	}
    }


    CmdLvs::Role
    CmdLvs::parse_role(const string& role) const
    {
	vector<string> tmp;
	boost::split(tmp, role, boost::is_any_of(","), boost::token_compress_on);

	return contains(tmp, "public") ? Role::PUBLIC : Role::PRIVATE;
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
    operator<<(std::ostream& s, const CmdLvs::Segment& segment)
    {
	if (segment.stripes != 0)
	    s << "stripes:" << segment.stripes;

	if (segment.stripe_size != 0)
	    s << " stripe-size:" << segment.stripe_size;

	if (segment.chunk_size != 0)
	    s << " chunk-size:" << segment.chunk_size;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLvs::Lv& lv)
    {
	s << "lv-name:" << lv.lv_name << " lv-uuid:" << lv.lv_uuid << " vg-name:"
	  << lv.vg_name << " vg-uuid:" << lv.vg_uuid << " lv-type:" << toString(lv.lv_type)
	  << " role:" << (lv.role == CmdLvs::Role::PUBLIC ? "public" : "private")
	  << " active:" << lv.active << " size:" << lv.size;

	if (lv.origin_size != 0)
	    s << " origin-size:" << lv.origin_size;

	if (!lv.pool_name.empty())
	    s << " pool-name:" << lv.pool_name;

	if (!lv.pool_uuid.empty())
	    s << " pool-uuid:" << lv.pool_uuid;

	if (!lv.origin_name.empty())
	    s << " origin-name:" << lv.origin_name;

	if (!lv.origin_uuid.empty())
	    s << " origin-uuid:" << lv.origin_uuid;

	if (!lv.data_name.empty())
	    s << " data-name:" << lv.data_name;

	if (!lv.data_uuid.empty())
	    s << " data-uuid:" << lv.data_uuid;

	if (!lv.metadata_name.empty())
	    s << " metadata-name:" << lv.metadata_name;

	if (!lv.metadata_uuid.empty())
	    s << " metadata-uuid:" << lv.metadata_uuid;

	s << " segments:" << lv.segments;

	return s;
    }


#define VGS_OPTIONS "vg_name,vg_uuid,vg_attr,vg_extent_size,vg_extent_count,vg_free_count"


    CmdVgs::CmdVgs()
    {
	SystemCmd cmd({ VGS_BIN, COMMON_LVM_OPTIONS, "--options", VGS_OPTIONS }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    CmdVgs::CmdVgs(const string& vg_name)
    {
	SystemCmd cmd({ VGS_BIN, COMMON_LVM_OPTIONS, "--options", VGS_OPTIONS, "--", vg_name },
		SystemCmd::DoThrow);

	parse(cmd.stdout());

	if (vgs.size() != 1)
	    ST_THROW(Exception("command vgs returned wrong number of vgs"));

	const Vg& vg = vgs[0];

	if (vg.vg_name != vg_name)
	    ST_THROW(Exception("command vgs returned wrong vg_name"));
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
