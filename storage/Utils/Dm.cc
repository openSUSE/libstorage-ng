/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#include "storage/Utils/Dm.h"


namespace storage
{

    // # dmsetup ls
    // stupid space    (254:13)
    // stupid$dollar   (254:14)

    // # ll /dev/mapper/
    // lrwxrwxrwx 1 root root       8 Jul 29 11:26 stupid\x20space -> ../dm-13
    // lrwxrwxrwx 1 root root       8 Jul 29 11:26 stupid\x24dollar -> ../dm-14

    string
    dm_encode(const string& s)
    {
	string r = s;

	string::size_type pos = 0;

	while (true)
	{
	    pos = r.find_first_of(" !$%*~", pos);
	    if (pos == string::npos)
		break;

	    char tmp[16];
	    sprintf(tmp, "\\x%02x", r[pos]);
	    r.replace(pos, 1, tmp);

	    pos += 4;
	}

	return r;
    }


}
