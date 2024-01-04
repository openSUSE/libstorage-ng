/*
 * Copyright (c) 2023 SUSE LLC
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


#ifndef STORAGE_CMD_NVME_H
#define STORAGE_CMD_NVME_H


#include <string>
#include <vector>

#include "storage/Devices/Disk.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{

    using std::string;
    using std::vector;


    /**
     * Class to run the command "nvme list". So far only used to log the output.
     */
    class CmdNvmeList
    {

    public:

	CmdNvmeList();

	friend std::ostream& operator<<(std::ostream& s, const CmdNvmeList& cmd_nvme_list);

    private:

	void parse(const vector<string>& lines);

    };


    /**
     * Class to run the command "nvme list-subsys".
     */
    class CmdNvmeListSubsys
    {

    public:

	CmdNvmeListSubsys();

	struct Path
	{
	    string name;
	    Transport transport = Transport::UNKNOWN;
	};

	struct Subsystem
	{
	    string name;
	    string nqn;
	    vector<Path> paths;
	};

	struct Something
	{
	    string host_nqn;
	    string host_id;
	    vector<Subsystem> subsystems;
	};

	/**
	 * Get the transport for a device. Checks whether the system is using native or
	 * non-native NVMe multipath.
	 */
	Transport get_transport(const string& device, SystemInfo::Impl& system_info) const;

	friend std::ostream& operator<<(std::ostream& s, const CmdNvmeListSubsys& cmd_nvme_list_subsys);
	friend std::ostream& operator<<(std::ostream& s, const Path& path);
	friend std::ostream& operator<<(std::ostream& s, const Subsystem& subsystem);
	friend std::ostream& operator<<(std::ostream& s, const Something& somethings);

    private:

	void parse(const vector<string>& lines);

	bool is_native_multipath(SystemInfo::Impl& system_info) const;

	const Subsystem& find_subsystem_by_name(const string& name) const;
	const Path& find_path_by_name(const string& name) const;

	vector<Something> somethings;

    };

}

#endif
