/*
 * Copyright (c) [2010-2014] Novell, Inc.
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


    Lsscsi::Lsscsi()
    {
	SystemCmd cmd(LSSCSIBIN " --transport");
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    void
    Lsscsi::parse(const vector<string>& lines)
    {
	data.clear();

	for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
	    string line = boost::replace_all_copy(*it, " usb: ", " usb:");

	    string type = extractNthWord(1, line);
	    if (type != "disk")
		continue;

	    string transport = extractNthWord(2, line);
	    string device = extractNthWord(3, line);
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

	    data[device] = entry;
	}

	y2mil(*this);
    }


    bool
    Lsscsi::getEntry(const string& device, Entry& entry) const
    {
	const_iterator i = data.find(device);
	if (i == data.end())
	    return false;

	entry = i->second;
	return true;
    }


    std::ostream& operator<<(std::ostream& s, const Lsscsi& lsscsi)
    {
	for (Lsscsi::const_iterator it = lsscsi.data.begin(); it != lsscsi.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << endl;

	return s;
    }


    std::ostream& operator<<(std::ostream& s, const Lsscsi::Entry& entry)
    {
	return s << "transport:" << toString(entry.transport);
    }

}
