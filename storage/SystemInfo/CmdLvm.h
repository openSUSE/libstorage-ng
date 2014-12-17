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


#ifndef CMD_LVM_H
#define CMD_LVM_H


#include <string>
#include <vector>
#include <list>


namespace storage
{
    using std::string;
    using std::vector;
    using std::list;


    class CmdVgs
    {
    public:

	CmdVgs(bool do_probe = true);

	void probe();

	friend std::ostream& operator<<(std::ostream& s, const CmdVgs& cmdvgs);

	void parse(const vector<string>& lines);

	const list<string>& getVgs() const { return vgs; }

    private:

	list<string> vgs;

    };


    class CmdVgdisplay
    {
    public:

	CmdVgdisplay(const string& name, bool do_probe = true);

	void probe();

	struct LvEntry
	{
	    void clear();

	    string name;
	    string uuid;
	    string status;
	    string origin;
	    string used_pool;
	    unsigned long num_le;
	    unsigned long num_cow_le;
	    unsigned long long pool_chunk;
	    bool read_only;
	    bool pool;
	};

	struct PvEntry
	{
	    void clear();

	    string device;
	    string uuid;
	    string status;
	    unsigned long num_pe;
	    unsigned long free_pe;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdVgdisplay& cmdvgdisplay);
	friend std::ostream& operator<<(std::ostream& s, const LvEntry& lv_entry);
	friend std::ostream& operator<<(std::ostream& s, const PvEntry& pv_entry);

	void parse(const vector<string>& lines);

	string name;
	string uuid;
	string status;
	unsigned long long pe_size;
	unsigned long num_pe;
	unsigned long free_pe;
	bool read_only;
	bool lvm1;

	list<LvEntry> lv_entries;
	list<PvEntry> pv_entries;

    };

}

#endif
