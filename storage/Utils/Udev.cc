/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2024] SUSE LLC
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


#include "storage/Utils/Udev.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    string
    udev_encode(const string& s)
    {
	string r = s;

	string::size_type pos = 0;

	while (true)
	{
	    pos = r.find_first_of(" '\\/$", pos);
	    if (pos == string::npos)
		break;

	    char tmp[16];
	    sprintf(tmp, "\\x%02x", r[pos]);
	    r.replace(pos, 1, tmp);

	    pos += 4;
	}

	return r;
    }


    string
    udev_decode(const string& s)
    {
	string r = s;

	string::size_type pos = 0;

	while (true)
	{
	    pos = r.find("\\x", pos);
	    if (pos == string::npos || pos > r.size() - 4)
		break;

	    unsigned int tmp;
	    if (sscanf(r.substr(pos + 2, 2).c_str(), "%x", &tmp) == 1)
		r.replace(pos, 4, 1, (char) tmp);

	    pos += 1;
	}

	return r;
    }


    void
    udev_settle()
    {
	SystemCmd({ UDEVADM_BIN_SETTLE }, SystemCmd::NoThrow);
    }


    void
    Udevadm::settle()
    {
	if (settle_needed)
	{
	    udev_settle();
	    settle_needed = false;
	}
    }


    void
    Udevadm::set_settle_needed()
    {
	settle_needed = true;
    }

}
