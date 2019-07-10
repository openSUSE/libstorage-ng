/*
 * Copyright (c) 2010 Novell, Inc.
 * Copyright (c) [2015-2019] SUSE LLC
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


#ifndef STORAGE_ENUM_H
#define STORAGE_ENUM_H


#include <string>
#include <vector>
#include <algorithm>

#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{
    using std::string;
    using std::vector;


    /**
     *  EnumTraits can be used for mapping of enum to string and vice versa. Order of
     *  strings in EnumTraits have to match order in Enum itself. For mapping is that used
     *  methods like toString or toValue. For examples see e.g. Disk Device or any other usage.
     */
    template <typename EnumType> struct EnumTraits {};


    template <typename EnumType>
    const string& toString(EnumType value)
    {
	static_assert(std::is_enum<EnumType>::value, "not enum");

	const vector<string>& names = EnumTraits<EnumType>::names;

	typedef typename std::underlying_type<EnumType>::type underlying_type;

	// Comparisons must not be done with type of enum since the enum may
	// define comparison operators.
	ST_CHECK_INDEX(static_cast<underlying_type>(value),
		       static_cast<underlying_type>(0),
		       static_cast<underlying_type>(names.size() - 1));

	return names[static_cast<underlying_type>(value)];
    }


    template <typename EnumType>
    bool toValue(const string& str, EnumType& value, bool log_error = true)
    {
	static_assert(std::is_enum<EnumType>::value, "not enum");

	const vector<string>& names = EnumTraits<EnumType>::names;

	vector<string>::const_iterator it = find(names.begin(), names.end(), str);

	if (it == names.end())
	{
	    if (log_error)
		y2err("converting '" << str << "' to enum failed");
	    return false;
	}

	value = EnumType(it - names.begin());
	return true;
    }


    template <typename EnumType>
    EnumType toValueWithFallback(const string& str, EnumType fallback, bool log_error = true)
    {
	static_assert(std::is_enum<EnumType>::value, "not enum");

	EnumType value(fallback);

	toValue(str, value, log_error);

	return value;
    }

}


#endif
