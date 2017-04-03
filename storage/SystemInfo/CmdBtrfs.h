/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) 2017 SUSE LLC
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


#ifndef STORAGE_CMD_BTRFS_H
#define STORAGE_CMD_BTRFS_H


#include <string>
#include <vector>
#include <map>

#include "storage/Filesystems/BtrfsSubvolumeImpl.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


    /**
     * Class to probe for btrfs filesystems: Call "btrfs filesystem show"
     * (globally, not restricted to any disk or partition) and parse its
     * output.
     */
    class CmdBtrfsFilesystemShow
    {
    public:

	/**
	 * Constructor: Probe for btrfs filesystems with the
	 * "btrfs filesystem show" command and parse its output.
	 *
	 * This may throw a SystemCmdException or a ParseException.
	 */
	CmdBtrfsFilesystemShow();

	/**
	 * Entry for one btrfs filesystem. Since btrfs includes a volume
	 * manager (independent of LVM or the device mapper), this may be
	 * multiple devices for a single btrfs filesystem.
	 */
	struct Entry
	{
	    vector<string> devices;
	};

	friend std::ostream& operator<<(std::ostream& s, const CmdBtrfsFilesystemShow& cmdbtrfsfilesystemshow);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	/**
	 * Return a vector of all filesystem UUIDs with btrfs.
	 */
	vector<string> get_uuids() const;

    private:

	/**
	 * Parse the output of "btrfs filesystem show" passed in 'lines'.
	 *
	 * This may throw a ParseException.
	 */
	void parse(const vector<string>& lines);

	typedef map<string, Entry>::const_iterator const_iterator;

	map<string, Entry> data;

    };


    /**
     * Class to probe for btrfs subvolumes: Call "btrfs subvolume list
     * <mountpoint>".
     */
    class CmdBtrfsSubvolumeList
    {
    public:

	typedef string key_t;

	CmdBtrfsSubvolumeList(const key_t& key, const string& mountpoint);

	struct Entry
	{
	    Entry()
		: id(BtrfsSubvolume::Impl::unknown_id),
		  parent_id(BtrfsSubvolume::Impl::unknown_id),
		  path() {}

	    long id;
	    long parent_id;
	    string path;
	};

	typedef vector<Entry>::value_type value_type;
	typedef vector<Entry>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	const_iterator find_entry_by_path(const string& path) const;

	friend std::ostream& operator<<(std::ostream& s, const CmdBtrfsSubvolumeList& cmdbtrfssubvolumelist);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

    private:

	void parse(const vector<string>& lines);

	vector<Entry> data;

    };


    /**
     * Class to probe for btrfs default subvolume: Call "btrfs subvolume
     * get-default <mountpoint>".
     */
    class CmdBtrfsSubvolumeGetDefault
    {
    public:

	typedef string key_t;

	CmdBtrfsSubvolumeGetDefault(const key_t& key, const string& mountpoint);

	long get_id() const { return id; }

	friend std::ostream& operator<<(std::ostream& s, const CmdBtrfsSubvolumeGetDefault&
					cmdbtrfssubvolumegetdefault);

    private:

	void parse(const vector<string>& lines);

	long id;

    };
}

#endif
