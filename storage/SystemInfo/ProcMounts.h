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


#ifndef STORAGE_PROC_MOUNTS_H
#define STORAGE_PROC_MOUNTS_H


#include <string>
#include <list>
#include <map>

#include "storage/EtcFstab.h"

namespace storage
{
    using std::list;
    using std::map;
    using std::multimap;

    class SystemCmd;

    class ProcMounts
    {
    public:

	ProcMounts();

	string getMount(const string& device) const;
	string getMount(const list<string>& devices) const;

	list<string> getAllMounts(const string& device) const;
	list<string> getAllMounts(const list<string>& devices) const;

	map<string, string> allMounts() const;

	list<FstabEntry> getEntries() const;

	friend std::ostream& operator<<(std::ostream& s, const ProcMounts& procmounts);

    protected:

	void parse(const vector<string>& mount_lines, const vector<string>& swap_lines);

	typedef multimap<string, FstabEntry>::const_iterator const_iterator;

	multimap<string, FstabEntry> data;

    };

}

#endif
