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


#ifndef STORAGE_CMD_LVM_H
#define STORAGE_CMD_LVM_H


#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;


    class CmdPvs
    {
    public:

	CmdPvs();

	struct Pv
	{
	    Pv() : pv_name(), pv_uuid(), vg_name(), vg_uuid() {}

	    string pv_name;
	    string pv_uuid;
	    string vg_name;
	    string vg_uuid;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdPvs& cmd_pvs);
	friend std::ostream& operator<<(std::ostream& s, const Pv& pv);

	const vector<Pv>& get_pvs() const { return pvs; }

	const Pv& find_by_pv_uuid(const string& pv_uuid) const;

    private:

	void parse(const vector<string>& lines);

	vector<Pv> pvs;

    };


    class CmdLvs
    {
    public:

	CmdLvs();

	struct Lv
	{
	    Lv() : lv_name(), lv_uuid(), vg_name(), vg_uuid(), active(false), size(0) {}

	    string lv_name;
	    string lv_uuid;
	    string vg_name;
	    string vg_uuid;
	    bool active;
	    unsigned long long size;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdLvs& cmd_lvs);
	friend std::ostream& operator<<(std::ostream& s, const Lv& lv);

	const vector<Lv>& get_lvs() const { return lvs; }

	const Lv& find_by_lv_uuid(const string& lv_uuid) const;

    private:

	void parse(const vector<string>& lines);

	vector<Lv> lvs;

    };


    class CmdVgs
    {
    public:

	CmdVgs();

	struct Vg
	{
	    Vg() : vg_name(), vg_uuid(), extent_size(0), extent_count(0) {}

	    string vg_name;
	    string vg_uuid;
	    unsigned long extent_size;
	    unsigned long extent_count;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdVgs& cmd_vgs);
	friend std::ostream& operator<<(std::ostream& s, const Vg& vg);

	const vector<Vg>& get_vgs() const { return vgs; }

	const Vg& find_by_vg_uuid(const string& vg_uuid) const;

    private:

	void parse(const vector<string>& lines);

	vector<Vg> vgs;

    };

}

#endif
