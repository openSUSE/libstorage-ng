/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2026] SUSE LLC
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
#include <list>
#include <vector>

#include "storage/Utils/Udev.h"
#include "storage/Filesystems/Filesystem.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{
    using std::string;
    using std::map;
    using std::list;
    using std::vector;


    /**
     * Run and parse the "blkid" command.
     */
    class Blkid
    {
    public:

	Blkid(Udevadm& udevadm);
	Blkid(Udevadm& udevadm, const string& device);

	struct Entry
	{
	    bool is_fs = false;
	    FsType fs_type = FsType::UNKNOWN;
	    string fs_uuid;
	    string fs_label;
	    string fs_journal_uuid;
	    string fs_sub_uuid;

	    bool is_journal = false;
	    string journal_uuid;

	    bool is_md = false;

	    bool is_lvm = false;

	    bool is_luks = false;
	    string luks_uuid;
	    string luks_label;

	    bool is_bitlocker = false;

	    bool is_bcache = false;
	    string bcache_uuid;
	};

	typedef map<string, Entry>::const_iterator const_iterator;
	typedef map<string, Entry>::value_type value_type;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	friend std::ostream& operator<<(std::ostream& s, const Blkid& blkid);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	/**
	 * Find an entry by any name including any symbolic links in
	 * /dev. Function might require a system lookup and is therefore
	 * slow.
	 */
	const_iterator find_by_any_name(const string& device, SystemInfo::Impl& system_info) const;

	const_iterator find_by_journal_uuid(const string& journal_uuid) const;

	/**
	 * Get the sole entry. Useful when constructor with device parameter
	 * was used.
	 */
	const_iterator get_sole_entry() const;

	bool any_md() const;
	bool any_lvm() const;
	bool any_luks() const;
	bool any_bitlocker() const;
	bool any_bcache() const;
	bool any_btrfs() const;

        static list<string> split_line( const string & line );

    private:

	void parse(const vector<string>& lines);

	map<string, Entry> data;

    };


    class CmdBlkidVersion
    {
    public:

	static void query_version();
	static void parse_version(const string& version);

	static bool supports_json_option_v2();

    private:

	static bool did_set_version;

	static int major;
	static int minor;
	static int patchlevel;

    };

}


#endif
