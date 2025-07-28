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


#include <regex>

#include "storage/SystemInfo/CmdNvme.h"
#include "storage/SystemInfo/DevAndSys.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/JsonFile.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    CmdNvmeList::CmdNvmeList()
    {
	SystemCmd cmd({ NVME_BIN, "list", "--verbose", "--output-format", "json" }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdNvmeList::parse(const vector<string>& lines)
    {
	JsonFile json_file(lines);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdNvmeList& cmd_nvme_list)
    {
	return s;
    }


    CmdNvmeListSubsys::CmdNvmeListSubsys()
    {
	SystemCmd cmd({ NVME_BIN, "list-subsys", "--verbose", "--output-format", "json" }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdNvmeListSubsys::parse(const vector<string>& lines)
    {
	JsonFile json_file(lines);

	string tmp;

	if (!json_object_is_type(json_file.get_root(), json_type_array))
	    ST_THROW(Exception("not an array in output of 'nvme list-subsys'"));

	for (size_t i = 0; i < json_object_array_length(json_file.get_root()); ++i)
	{
	    json_object* tmp1 = json_object_array_get_idx(json_file.get_root(), i);

	    Something something;

	    if (!get_child_value(tmp1, "HostNQN", something.host_nqn))
		ST_THROW(Exception("\"HostNQN\" not found in json output of 'nvme list-subsys'"));

	    // Using "Host ID" instead of "HostID" was a mistake in nvme-cli and reverted, see bsc #1218306.
	    if (!get_child_value(tmp1, "HostID", something.host_id) &&
		!get_child_value(tmp1, "Host ID", something.host_id))
		ST_THROW(Exception("\"HostID\" not found in json output of 'nvme list-subsys'"));

	    vector<json_object*> tmp2;
	    if (!get_child_nodes(tmp1, "Subsystems", tmp2))
		ST_THROW(Exception("\"Controllers\" not found in json output of 'nvme list-subsys'"));

	    for (json_object* tmp3 : tmp2)
	    {
		Subsystem subsystem;

		if (!get_child_value(tmp3, "Name", subsystem.name))
		    ST_THROW(Exception("\"Name\" not found in json output of 'nvme list-subsys'"));

		if (!get_child_value(tmp3, "NQN", subsystem.nqn))
		    ST_THROW(Exception("\"NQN\" not found in json output of 'nvme list-subsys'"));

		vector<json_object*> tmp4;
		if (!get_child_nodes(tmp3, "Paths", tmp4))
		    ST_THROW(Exception("\"Paths\" not found in json output of 'nvme list-subsys'"));

		for (json_object* tmp5 : tmp4)
		{
		    Path path;

		    if (!get_child_value(tmp5, "Name", path.name))
			ST_THROW(Exception("\"Name\" not found in json output of 'nvme list-subsys'"));

		    if (!get_child_value(tmp5, "Transport", tmp))
			ST_THROW(Exception("\"Transport\" not found in json output of 'nvme list'"));

		    if (tmp == "pcie")
			path.transport = Transport::PCIE;
		    else if (tmp == "fc")
			path.transport = Transport::FC;
		    else if (tmp == "tcp")
			path.transport = Transport::TCP;
		    else if (tmp == "rdma")
			path.transport = Transport::RDMA;
		    else if (tmp == "loop")
			path.transport = Transport::LOOP;
		    else
			y2err("unknown NVMe transport");

		    subsystem.paths.push_back(path);
		}

		something.subsystems.push_back(subsystem);
	    }

	    somethings.push_back(something);
	}

	y2mil(*this);
    }


    Transport
    CmdNvmeListSubsys::get_transport(const string& device, SystemInfo::Impl& system_info) const
    {
	static const regex nvme_name_rx(DEV_DIR "/nvme([0-9]+)n([0-9]+)", regex::extended);

	smatch match;

	if (!regex_match(device, match, nvme_name_rx))
	    ST_THROW(Exception("failed to extract nvme subsystem/path"));

	if (is_native_multipath(system_info))
	{
	    string name = "nvme-subsys" + match[1].str();
	    const CmdNvmeListSubsys::Subsystem& subsystem = find_subsystem_by_name(name);

	    if (subsystem.paths.empty())
		ST_THROW(Exception("nvme subsystem has no paths"));

	    // TODO in theory there could be several paths with different transports
	    return subsystem.paths[0].transport;
	}
	else
	{
	    string name = "nvme" + match[1].str();
	    const CmdNvmeListSubsys::Path& path = find_path_by_name(name);

	    return path.transport;
	}
    }


    bool
    CmdNvmeListSubsys::is_native_multipath(SystemInfo::Impl& system_info) const
    {
	const File& multipath_file = system_info.getFile(SYSFS_DIR "/module/nvme_core/parameters/multipath");
	return multipath_file.get<string>() == "Y";
    }


    const CmdNvmeListSubsys::Subsystem&
    CmdNvmeListSubsys::find_subsystem_by_name(const string& name) const
    {
	for (const Something& something : somethings)
	{
	    for (const Subsystem& subsystem : something.subsystems)
	    {
		if (subsystem.name == name)
		    return subsystem;
	    }
	}

	ST_THROW(Exception("nvme subsystem not found"));
    }


    const CmdNvmeListSubsys::Path&
    CmdNvmeListSubsys::find_path_by_name(const string& name) const
    {
	for (const Something& something : somethings)
	{
	    for (const Subsystem& subsystem : something.subsystems)
	    {
		for (const Path& path : subsystem.paths)
		{
		    if (path.name == name)
			return path;
		}
	    }
	}

	ST_THROW(Exception("nvme path not found"));
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdNvmeListSubsys& cmd_nvme_list_subsys)
    {
	for (const CmdNvmeListSubsys::Something& something : cmd_nvme_list_subsys.somethings)
	    s << something << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdNvmeListSubsys::Something& something)
    {
	s << "host-nqn:" << something.host_nqn << ", host-id:" << something.host_id
	  << ", subsystems: " << something.subsystems;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdNvmeListSubsys::Subsystem& subsystem)
    {
	s << "name:" << subsystem.name << "nqn:" << subsystem.nqn
	  << ", paths:" << subsystem.paths;

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdNvmeListSubsys::Path& path)
    {
	s << "name:" << path.name << ", transport: "
	  << get_transport_name(path.transport);

	return s;
    }

}
