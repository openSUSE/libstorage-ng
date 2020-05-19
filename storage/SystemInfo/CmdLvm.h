/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_CMD_LVM_H
#define STORAGE_CMD_LVM_H


#include <string>
#include <vector>

#include "storage/Devices/LvmLv.h"
#include "storage/Utils/JsonFile.h"


namespace storage
{
    using std::string;
    using std::vector;


    class CmdLvm
    {
    protected:

	virtual ~CmdLvm() {}

	void parse(const vector<string>& lines, const char* tag);
	virtual void parse(json_object* object) = 0;

    };


    class CmdPvs : protected CmdLvm
    {
    public:

	CmdPvs();

	struct Pv
	{
	    Pv() : pv_name(), pv_uuid(), vg_name(), vg_uuid(), pe_start(0), missing(false), duplicate(false) {}

	    string pv_name;
	    string pv_uuid;
	    string vg_name;
	    string vg_uuid;
	    unsigned long long pe_start;

	    bool missing;
	    bool duplicate;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdPvs& cmd_pvs);
	friend std::ostream& operator<<(std::ostream& s, const Pv& pv);

	const vector<Pv>& get_pvs() const { return pvs; }

	const Pv& find_by_pv_uuid(const string& pv_uuid) const;

    private:

	void parse(const vector<string>& lines);
	virtual void parse(json_object* object) override;

	vector<Pv> pvs;

    };


    class CmdLvs : protected CmdLvm
    {
    public:

	CmdLvs();

	/**
	 * Enum to represent the role reported by lvs. So far only
	 * public and private are needed.
	 */
	enum class Role { PUBLIC, PRIVATE };

	struct Segment
	{
	    Segment() : stripes(0), stripe_size(0), chunk_size(0) {}

	    unsigned long stripes;
	    unsigned long long stripe_size;
	    unsigned long long chunk_size;
	};

	struct Lv
	{
	    Lv() : lv_name(), lv_uuid(), vg_name(), vg_uuid(), lv_type(LvType::UNKNOWN),
		   role(Role::PRIVATE), active(false), size(0), origin_size(0), pool_name(),
		   pool_uuid(), origin_name(), origin_uuid(), data_name(), data_uuid(),
		   metadata_name(), metadata_uuid() {}

	    string lv_name;
	    string lv_uuid;
	    string vg_name;
	    string vg_uuid;
	    LvType lv_type;
	    Role role;
	    bool active;
	    unsigned long long size;
	    unsigned long long origin_size;
	    string pool_name;
	    string pool_uuid;
	    string origin_name;
	    string origin_uuid;
	    string data_name;
	    string data_uuid;
	    string metadata_name;
	    string metadata_uuid;

	    vector<Segment> segments;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdLvs& cmd_lvs);
	friend std::ostream& operator<<(std::ostream& s, const Segment& segment);
	friend std::ostream& operator<<(std::ostream& s, const Lv& lv);

	const vector<Lv>& get_lvs() const { return lvs; }

	const Lv& find_by_lv_uuid(const string& lv_uuid) const;

	size_t number_of_inactive() const;

    private:

	void parse(const vector<string>& lines);
	virtual void parse(json_object* object) override;
	Role parse_role(const string& role) const;

	vector<Lv> lvs;

    };


    class CmdVgs : protected CmdLvm
    {
    public:

	CmdVgs();

	struct Vg
	{
	    Vg() : vg_name(), vg_uuid(), extent_size(0), extent_count(0), free_extent_count(0) {}

	    string vg_name;
	    string vg_uuid;
	    unsigned long extent_size;
	    unsigned long extent_count;
	    unsigned long free_extent_count;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdVgs& cmd_vgs);
	friend std::ostream& operator<<(std::ostream& s, const Vg& vg);

	const vector<Vg>& get_vgs() const { return vgs; }

	const Vg& find_by_vg_uuid(const string& vg_uuid) const;

    private:

	void parse(const vector<string>& lines);
	virtual void parse(json_object* object) override;

	vector<Vg> vgs;

    };

}

#endif
