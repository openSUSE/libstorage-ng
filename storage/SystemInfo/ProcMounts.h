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


#ifndef STORAGE_PROC_MOUNTS_H
#define STORAGE_PROC_MOUNTS_H


#include <string>
#include <vector>
#include <map>

#include "storage/EtcFstab.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::multimap;


    class ProcMounts
    {
    public:

	ProcMounts();
	~ProcMounts();

	/**
	 * Return all entries for the device. Aliases, e.g. udev symlinks, are handled by
	 * the function.
	 *
	 * This object keeps ownership of the entries; do not delete them.
	 */
	vector<const FstabEntry*> get_by_name(const string& name, SystemInfo::Impl& system_info) const;

	/**
	 * Return all NFS and NFS4 entries.
	 *
	 * This object keeps ownership of the entries; do not delete them.
	 */
	vector<const FstabEntry*> get_all_nfs() const;

	/**
	 * Return all tmpfs entries.
	 *
	 * This object keeps ownership of the entries; do not delete them.
	 */
	vector<const FstabEntry*> get_all_tmpfs() const;

	friend std::ostream& operator<<(std::ostream& s, const ProcMounts& proc_mounts);

    private:

	void parse_proc_mounts(const vector<string>& lines);
	void parse_proc_swaps(const vector<string>& lines);

	using value_type = multimap<string, FstabEntry>::value_type;

	multimap<string, FstabEntry> data;

    };

}

#endif
