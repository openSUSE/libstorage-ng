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


#ifndef STORAGE_CMD_BLKID_H
#define STORAGE_CMD_BLKID_H


#include <string>
#include <map>
#include <vector>

#include "storage/Filesystems/Filesystem.h"


namespace storage
{
    using std::string;
    using std::map;
    using std::vector;


    class SystemInfo;


    class Blkid
    {
    public:

	Blkid();
	Blkid(const string& device);

	struct Entry
	{
	    Entry() : is_fs(false), fs_type(FsType::UNKNOWN), fs_uuid(), fs_label(),
		      is_md(false), is_lvm(false), is_luks(false), luks_uuid(),
		      is_bcache(false), bcache_uuid() {}

	    bool is_fs;
	    FsType fs_type;
	    string fs_uuid;
	    string fs_label;

	    bool is_md;

	    bool is_lvm;

	    bool is_luks;
	    string luks_uuid;

	    bool is_bcache;
	    string bcache_uuid;
	};

	friend std::ostream& operator<<(std::ostream& s, const Blkid& blkid);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	bool find_by_name(const string& device, Entry& entry, SystemInfo& systeminfo) const;

	bool any_md() const;
	bool any_lvm() const;
	bool any_luks() const;
	bool any_bcache() const;

    private:

	void parse(const vector<string>& lines);

	typedef map<string, Entry>::const_iterator const_iterator;
	typedef map<string, Entry>::value_type value_type;

	map<string, Entry> data;

    };

}


#endif
