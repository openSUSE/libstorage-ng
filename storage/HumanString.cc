/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#include "storage/Utils/HumanString.h"


namespace storage_legacy
{
    using namespace storage;


    int
    numSuffixes()
    {
	return num_suffixes();
    }


    string
    getSuffix(int i, bool classic)
    {
	return get_suffix(i, classic);
    }


    string
    byteToHumanString(unsigned long long size, bool classic, int precision, bool omit_zeroes)
    {
	return byte_to_humanstring(size, classic, precision, omit_zeroes);
    }


    bool
    humanStringToByte(const string& str, bool classic, unsigned long long& size)
    {
	try
	{
	    size = humanstring_to_byte(str, classic);
	    return true;
	}
	catch (const ParseError&)
	{
	    return false;
	}
    }

}
