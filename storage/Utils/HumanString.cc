/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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


#include <locale>
#include <cmath>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/Text.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/Math.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{
    using std::string;


    int
    num_suffixes()
    {
	return 7;
    }


    vector<Text>
    get_all_suffixes(int i, bool all = false, bool sloppy = false)
    {
	vector<Text> ret;

	switch (i)
	{
	    case 0:
		// TRANSLATORS: symbol for "bytes" (best keep untranslated)
		ret.push_back(_("B"));
		if (sloppy)
		    ret.push_back(Text("", ""));
		break;

	    case 1:
		// TRANSLATORS: symbol for "kibi bytes" (best keep untranslated)
		ret.push_back(_("KiB"));
		if (all)
		    // TRANSLATORS: symbol for "kilo bytes" (best keep untranslated)
		    ret.push_back(_("kB"));
		if (sloppy)
		    // TRANSLATORS: symbol for "kilo" (best keep untranslated)
		    ret.push_back(_("k"));
		break;

	    case 2:
		// TRANSLATORS: symbol for "mebi bytes" (best keep untranslated)
		ret.push_back(_("MiB"));
		if (all)
		    // TRANSLATORS: symbol for "mega bytes" (best keep untranslated)
		    ret.push_back(_("MB"));
		if (sloppy)
		    // TRANSLATORS: symbol for "mega" (best keep untranslated)
		    ret.push_back(_("M"));
		break;

	    case 3:
		// TRANSLATORS: symbol for "gibi bytes" (best keep untranslated)
		ret.push_back(_("GiB"));
		if (all)
		    // TRANSLATORS: symbol for "giga bytes" (best keep untranslated)
		    ret.push_back(_("GB"));
		if (sloppy)
		    // TRANSLATORS: symbol for "giga" (best keep untranslated)
		    ret.push_back(_("G"));
		break;

	    case 4:
		// TRANSLATORS: symbol for "tebi bytes" (best keep untranslated)
		ret.push_back(_("TiB"));
		if (all)
		    // TRANSLATORS: symbol for "tera bytes" (best keep untranslated)
		    ret.push_back(_("TB"));
		if (sloppy)
		    // TRANSLATORS: symbol for "tera" (best keep untranslated)
		    ret.push_back(_("T"));
		break;

	    case 5:
		// TRANSLATORS: symbol for "pebi bytes" (best keep untranslated)
		ret.push_back(_("PiB"));
		if (all)
		    // TRANSLATORS: symbol for "peta bytes" (best keep untranslated)
		    ret.push_back(_("PB"));
		if (sloppy)
		    // TRANSLATORS: symbol for "peta" (best keep untranslated)
		    ret.push_back(_("P"));
		break;

	    case 6:
		// TRANSLATORS: symbol for "exbi bytes" (best keep untranslated)
		ret.push_back(_("EiB"));
		if (all)
		    // TRANSLATORS: symbol for "exa bytes" (best keep untranslated)
		    ret.push_back(_("EB"));
		if (sloppy)
		    // TRANSLATORS: symbol for "exa" (best keep untranslated)
		    ret.push_back(_("E"));
		break;
	}

	return ret;
    }


    string
    get_suffix(int i, bool classic)
    {
	return classic ? get_all_suffixes(i).front().native : get_all_suffixes(i).front().translated;
    }


    // Both byte_to_humanstring and humanstring_to_byte use integer arithmetic
    // as long as possible to provide exact results for those cases.


    string
    byte_to_humanstring(unsigned long long size, bool classic, int precision, bool omit_zeroes)
    {
	const locale loc = classic ? locale::classic() : locale();

	// Calculate the index of the suffix to use. The index increases by 1
	// when the number of leading zeros decreases by 10.

	int i = size > 0 ? (64 - (clz(size) + 1)) / 10 : 0;

	if ((i == 0) || (omit_zeroes && is_multiple_of(size, 1ULL << 10 * i)))
	{
	    unsigned long long v = size >> 10 * i;

	    std::ostringstream s;
	    s.imbue(loc);
	    s << v << ' ' << get_suffix(i, classic);
	    return s.str();
	}
	else
	{
	    long double v = std::ldexp((long double)(size), - 10 * i);

	    std::ostringstream s;
	    s.imbue(loc);
	    s.setf(ios::fixed);
	    s.precision(precision);
	    s << v << ' ' << get_suffix(i, classic);
	    return s.str();
	}
    }


    namespace
    {
	// Helper functions to parse a number as int or float, multiply
	// according to the suffix. Do all required error checks.

	pair<bool, unsigned long long>
	parse_i(const string& str, int i, const locale& loc)
	{
	    istringstream s(str);
	    s.imbue(loc);

	    unsigned long long v;
	    s >> v;

	    if (!s.eof())
		return make_pair(false, 0);

	    if (s.fail())
	    {
		if (v == std::numeric_limits<unsigned long long>::max())
		    ST_THROW(OverflowException());

		return make_pair(false, 0);
	    }

	    if (v != 0 && str[0] == '-')
		ST_THROW(OverflowException());

	    if (v > 0 && clz(v) < 10 * i)
		ST_THROW(OverflowException());

	    v <<= 10 * i;

	    return make_pair(true, v);
	}


	pair<bool, unsigned long long>
	parse_f(const string& str, int i, const locale& loc)
	{
	    istringstream s(str);
	    s.imbue(loc);

	    long double v;
	    s >> v;

	    if (!s.eof())
		return make_pair(false, 0);

	    if (s.fail())
	    {
		if (v == std::numeric_limits<long double>::max())
		    ST_THROW(OverflowException());

		return make_pair(false, 0);
	    }

	    if (v < 0.0)
		ST_THROW(OverflowException());

	    v = std::round(std::ldexp(v, 10 * i));

	    if (v > std::numeric_limits<unsigned long long>::max())
		ST_THROW(OverflowException());

	    return make_pair(true, v);
	}

    }


    unsigned long long
    humanstring_to_byte(const string& str, bool classic)
    {
	const locale loc = classic ? locale::classic() : locale();

	const string str_trimmed = boost::trim_copy(str, loc);

	for (int i = 0; i < num_suffixes(); ++i)
	{
	    vector<Text> suffixes = get_all_suffixes(i, true, !classic);

	    for (const Text& suffix : suffixes)
	    {
		const string& tmp = classic ? suffix.native : suffix.translated;

		if (boost::iends_with(str_trimmed, tmp, loc))
		{
		    string number = boost::trim_copy(str_trimmed.substr(0, str_trimmed.size() - tmp.size()), loc);

		    pair<bool, unsigned long long> v;

		    v = parse_i(number, i, loc);
		    if (v.first)
			return v.second;

		    v = parse_f(number, i, loc);
		    if (v.first)
			return v.second;
		}
	    }
	}

	ST_THROW(ParseException("failed to parse", str, "something like 1 GiB"));
    }

}
