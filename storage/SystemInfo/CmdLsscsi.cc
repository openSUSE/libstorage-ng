/*
 * Copyright (c) [2010-2020] Novell, Inc.
 * Copyright (c) [2023-2025] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/CmdLsscsi.h"
#include "storage/Devices/DiskImpl.h"


namespace storage
{
    using namespace std;


    CmdLsscsi::CmdLsscsi()
    {
	const bool json = CmdLsscsiVersion::supports_json_option();

	SystemCmd cmd({ LSSCSI_BIN, "--transport" }, SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdLsscsi::parse(const vector<string>& lines)
    {
	data.clear();

	for (const string& line : lines)
	{
	    string type = extractNthWord(1, line);
	    if (type != "disk")
		continue;

	    string tmp = line;

	    boost::replace_all(tmp, " usb: ", " usb:");
	    boost::replace_all(tmp, " pcie 0x", " pcie:0x");

	    string transport = extractNthWord(2, tmp);
	    string device = extractNthWord(3, tmp);
	    if (boost::starts_with(transport, "/dev/"))
	    {
		device = transport;
		transport = "";
	    }

	    if (!boost::starts_with(device, "/dev/"))
		continue;

	    Entry entry;

	    if (boost::starts_with(transport, "sbp:"))
		entry.transport = Transport::SBP;
	    else if (boost::starts_with(transport, "ata:"))
		entry.transport = Transport::ATA;
	    else if (boost::starts_with(transport, "fc:"))
		entry.transport = Transport::FC;
	    else if (boost::starts_with(transport, "fcoe:"))
		entry.transport = Transport::FCOE;
	    else if (boost::starts_with(transport, "iqn"))
		entry.transport = Transport::ISCSI;
	    else if (boost::starts_with(transport, "sas:"))
		entry.transport = Transport::SAS;
	    else if (boost::starts_with(transport, "sata:"))
		entry.transport = Transport::SATA;
	    else if (boost::starts_with(transport, "spi:"))
		entry.transport = Transport::SPI;
	    else if (boost::starts_with(transport, "usb:"))
		entry.transport = Transport::USB;
	    else if (boost::starts_with(transport, "pcie:"))
		entry.transport = Transport::PCIE;

	    data[device] = entry;
	}

	y2mil(*this);
    }


    bool
    CmdLsscsi::get_entry(const string& device, Entry& entry) const
    {
	const_iterator it = data.find(device);
	if (it == data.end())
	    return false;

	entry = it->second;
	return true;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLsscsi& cmd_lsscsi)
    {
	for (CmdLsscsi::const_iterator it = cmd_lsscsi.data.begin(); it != cmd_lsscsi.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLsscsi::Entry& entry)
    {
	return s << "transport:" << toString(entry.transport);
    }


    void
    CmdLsscsiVersion::query_version()
    {
	if (did_set_version)
	    return;

	SystemCmd cmd({ LSSCSI_BIN, "--version" }, SystemCmd::DoThrow);
	if (cmd.stderr().empty())
	    ST_THROW(SystemCmdException(&cmd, "failed to query lsscsi version"));

	parse_version(cmd.stderr()[0]);
    }


    void
    CmdLsscsiVersion::parse_version(const string& version)
    {
	const regex version_rx("(version|release): ([0-9]+)\\.([0-9]+)", regex::extended);

	smatch match;

	if (!regex_search(version, match, version_rx))
	    ST_THROW(Exception("failed to parse lsscsi version '" + version + "'"));

	major = stoi(match[2]);
	minor = stoi(match[3]);

	y2mil("major:" << major << " minor:" << minor);

	did_set_version = true;
    }


    bool
    CmdLsscsiVersion::supports_json_option()
    {
	query_version();

	return major >= 1 || (major == 0 && minor >= 33);
    }


    bool CmdLsscsiVersion::did_set_version = false;

    int CmdLsscsiVersion::major = 0;
    int CmdLsscsiVersion::minor = 0;

}
