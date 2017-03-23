/*
 * Copyright (c) [2004-2010] Novell, Inc.
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


#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/EtcMdadm.h"
#include "storage/Devices/Disk.h"
#include "storage/Storage.h"
#include "storage/DevicegraphImpl.h"


namespace storage
{
    using namespace std;


    EtcMdadm::EtcMdadm(const Storage* storage, const string& filename)
	: storage(storage), mdadm(filename)
    {
    }


    bool
    EtcMdadm::update_entry(const mdconf_info& info)
    {
	y2mil("uuid:" << info.uuid << " device:" << info.device);

	if (info.uuid.empty())
	{
	    y2err("empty UUID " << info);
	    return false;
	}

	if (info.container_present && info.container_uuid.empty())
	{
	    y2err("empty UUID for container " << info);
	    return false;
	}

	if (info.container_present)
	    set_array_line(cont_line(info), info.container_uuid);

	set_array_line(array_line(info), info.uuid);

	set_device_line("DEVICE containers partitions");

	if (has_iscsi())
	    set_auto_line("AUTO -all");

	mdadm.save();

	return true;
    }


    bool
    EtcMdadm::remove_entry(const string& uuid)
    {
	y2mil("uuid:" << uuid);

	if (uuid.empty())
	{
	    y2err("empty UUID");
	    return false;
	}

	vector<string>& lines = mdadm.get_lines();
	vector<string>::iterator it = find_array(uuid);
	if (it == lines.end())
	{
	    y2war("line not found");
	    return false;
	}

	lines.erase(it);

	mdadm.save();

	return true;
    }


    void
    EtcMdadm::set_device_line(const string& line)
    {
	vector<string>& lines = mdadm.get_lines();
	vector<string>::iterator it = find_if(lines, string_starts_with("DEVICE"));
	if (it == lines.end())
	    lines.insert(lines.begin(), line);
	else
	    *it = line;
    }


    void
    EtcMdadm::set_auto_line(const string& line)
    {
	vector<string>& lines = mdadm.get_lines();
	vector<string>::iterator it = find_if(lines, string_starts_with("AUTO"));
	if (it == lines.end())
	    lines.insert(lines.begin(), line);
	else
	    *it = line;
    }


    void
    EtcMdadm::set_array_line(const string& line, const string& uuid)
    {
	vector<string>& lines = mdadm.get_lines();
	vector<string>::iterator it = find_array(uuid);
	if (it == lines.end())
	    lines.push_back(line);
	else
	    *it = line;
    }


    string
    EtcMdadm::cont_line(const mdconf_info& info) const
    {
	string line = "ARRAY";
	line += " metadata=" + info.container_metadata;
	line += " UUID=" + info.container_uuid;
	return line;
    }


    string
    EtcMdadm::array_line(const mdconf_info& info) const
    {
	string line = "ARRAY " + info.device;
	if (info.container_present)
	{
	    line += " container=" + info.container_uuid;
	    line += " member=" + info.container_member;
	}
	line += " UUID=" + info.uuid;
	return line;
    }


    vector<string>::iterator
    EtcMdadm::find_array(const string& uuid)
    {
	vector<string>& lines = mdadm.get_lines();
	for (vector<string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
	    if (boost::starts_with(*it, "ARRAY"))
	    {
		string tmp = get_uuid(*it);
		if (!tmp.empty() && tmp == uuid)
		    return it;
	    }
	}

	return lines.end();
    }


    string
    EtcMdadm::get_uuid(const string& line) const
    {
	string::size_type pos1 = line.find("UUID=");
	if (pos1 == string::npos)
	    return "";

	pos1 += 5;
	string::size_type pos2 = line.find_first_not_of("0123456789abcdefABCDEF:", pos1);
	return line.substr(pos1, pos2 - pos1);
    }


    bool
    EtcMdadm::has_iscsi() const
    {
	for (const Disk* disk : Disk::get_all(storage->get_probed()))
	{
	    if (disk->get_transport() == Transport::ISCSI)
		return true;
	}

	return false;
    }


    std::ostream&
    operator<<(std::ostream& s, const EtcMdadm::mdconf_info& info)
    {
	s << "device:" << info.device << " uuid:" << info.uuid;

	if (info.container_present)
	{
	    s << " container_present container_uuid:" << info.container_uuid << " container_member:"
	      << info.container_member << " container_metadata:" << info.container_metadata;
	}

	return s;
    }
}
