/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#include "storage/Actions/Base.h"


namespace storage
{

    namespace Action
    {

	string
	Base::details() const
	{
	    string ret;

	    switch (affect)
	    {
		case Affect::DEVICE:
		    ret = "sid:" + to_string(sid);
		    break;

		case Affect::HOLDER:
		    ret = "source-sid:" + to_string(sid_pair.first) + ", target-sid:" + to_string(sid_pair.second);
		    break;
	    }

	    if (first)
		ret += ", first";

	    if (last)
		ret += ", last";

	    if (only_sync)
		ret += ", only-sync";

	    if (nop)
		ret += ", nop";

	    return ret;
	}


	string
	Base::debug_text(const CommitData& commit_data) const
	{
	    string ret = text(commit_data).native;

	    if (nop)
		ret += " [nop]";

	    return ret;
	}

    }

}
