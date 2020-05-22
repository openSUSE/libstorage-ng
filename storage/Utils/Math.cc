/*
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


#include "storage/Utils/Math.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    bool
    is_multiple_of(unsigned long long i, unsigned long long j)
    {
	if (j == 0)
	    ST_THROW(Exception("divisor is zero"));

	return i % j == 0;
    }


    unsigned long long
    round_down(unsigned long long i, unsigned long long m)
    {
	if (m == 0)
	    ST_THROW(Exception("divisor is zero"));

	unsigned long long r = i % m;
	return i - r;
    }


    unsigned long long
    round_up(unsigned long long i, unsigned long long m)
    {
	if (m == 0)
	    ST_THROW(Exception("divisor is zero"));

	unsigned long long r = i % m;
	return r == 0 ? i : i - r + m;
    }

}
