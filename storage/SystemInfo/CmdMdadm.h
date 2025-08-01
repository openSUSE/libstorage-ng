/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2017-2022] SUSE LLC
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


#ifndef STORAGE_CMD_MDADM_H
#define STORAGE_CMD_MDADM_H


#include <string>
#include <vector>
#include <map>

#include "storage/Devices/Md.h"


namespace storage
{
    using std::string;
    using std::map;
    using std::vector;


    /**
     * Parse (the --export variant of) mdadm --detail
     */
    class CmdMdadmDetail
    {
    public:

	CmdMdadmDetail(const string& device);

	/**
	 * The UUID.
	 *
	 * Note: Special MD RAID format.
	 */
	string uuid;

	string devname;
	string metadata;
	MdLevel level = MdLevel::UNKNOWN;

	/**
	 * Mapping from device name to role (a number or spare). Faulty and journal
	 * devices are also marked as spare by mdadm here (that might be a bug).
	 */
	map<string, string> roles;

	friend std::ostream& operator<<(std::ostream& s, const CmdMdadmDetail& cmd_mdadm_detail);

    private:

	void parse(const vector<string>& lines);

	string device;

    };

}


#endif
