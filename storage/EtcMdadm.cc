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


namespace storage
{
    using namespace std;


    EtcMdadm::EtcMdadm(const string& filename)
	: mdadm(filename)
    {
    }


    void
    EtcMdadm::init(const Storage* storage)
    {
	set_device_line("DEVICE containers partitions");

        if (has_iscsi(storage))
            set_auto_line("AUTO -all");
    }


    bool
    EtcMdadm::has_entry(const string& uuid) const
    {
	return find_array(uuid) != mdadm.get_lines().end();
    }


    bool
    EtcMdadm::update_entry(const Entry& entry)
    {
	y2mil("uuid:" << entry.uuid << " device:" << entry.device);

	if (entry.uuid.empty())
	{
	    y2err("empty UUID " << entry);
	    return false;
	}

	set_array_line(array_line(entry), entry.uuid);

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
    EtcMdadm::array_line(const Entry& entry) const
    {
	string line = "ARRAY";

	if (!entry.device.empty())
	    line += " " + entry.device;

	if (!entry.metadata.empty())
	    line += " metadata=" + entry.metadata;

	if (!entry.container_uuid.empty())
	    line += " container=" + entry.container_uuid;

	if (!entry.container_member.empty())
	    line += " member=" + entry.container_member;

	line += " UUID=" + entry.uuid;

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


    vector<string>::const_iterator
    EtcMdadm::find_array(const string& uuid) const
    {
	const vector<string>& lines = mdadm.get_lines();
	for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
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
    EtcMdadm::has_iscsi(const Storage* storage) const
    {
	for (const Disk* disk : Disk::get_all(storage->get_probed()))
	{
	    if (disk->get_transport() == Transport::ISCSI)
		return true;
	}

	return false;
    }


    std::ostream&
    operator<<(std::ostream& s, const EtcMdadm::Entry& entry)
    {
	s << "device:" << entry.device << " uuid:" << entry.uuid;

	if (!entry.metadata.empty())
	    s << " metadata:" << entry.metadata;

	if (!entry.container_uuid.empty())
	    s << " container-uuid:" << entry.container_uuid;

	if (!entry.container_member.empty())
	    s << " container-member:" << entry.container_member;

	return s;
    }
}
