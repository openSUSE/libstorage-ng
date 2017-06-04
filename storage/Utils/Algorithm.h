/*
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


#ifndef STORAGE_ALGORITHM_H
#define STORAGE_ALGORITHM_H


#include <vector>


namespace storage
{

    using std::vector;


    /**
     * Return the first missing number of the numbers in the vector starting
     * from start. The numbers of the vector must be sorted and unique. The
     * objects of the vector must have a get_number() function.
     */
    template<typename Type>
    unsigned int
    first_missing_number(const vector<Type>& values, unsigned int start)
    {
	unsigned int number = start;

	for (const Type value : values)
	{
	    if (number != value->get_number())
		return number;

	    ++number;
	}

	return number;
    }

}


#endif
