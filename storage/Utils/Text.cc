/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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


#include <libintl.h>

#include "storage/Utils/Text.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Format.h"


namespace storage
{
    using namespace std;


    void
    Text::clear()
    {
	native.clear();
	translated.clear();
    }


    const Text&
    Text::operator+=(const Text& a)
    {
	native += a.native;
	translated += a.translated;
	return *this;
    }


    Text
    _(const char* msgid)
    {
	return Text(msgid, dgettext("libstorage-ng", msgid));
    }


    Text
    _(const char* msgid, const char* msgid_plural, unsigned long int n)
    {
	return Text(n == 1 ? msgid : msgid_plural, dngettext("libstorage-ng", msgid, msgid_plural, n));
    }


    Text
    join(const vector<string>& values, JoinMode join_mode, size_t limit)
    {
	vector<Text> tmp;

	transform(values.begin(), values.end(), back_inserter(tmp),
	    [](const string& value) { return UntranslatedText(value); }
	);

	return join(tmp, join_mode, limit);
    }


    Text
    join(const vector<Text>& values, JoinMode join_mode, size_t limit)
    {
	// number of texts in values
	size_t n1 = values.size();

	// number of texts to include in result, limit is at least 2
	size_t n2 = limit == 0 ? n1 : min(n1, max(limit, (size_t)(2)));

	Text ret;

	for (size_t i = 0; i < n2; ++i)
	{
	    if (i == 0)
	    {
		// first text to include in result

		ret = values[i];
	    }
	    else if (i < n2 - 1)
	    {
		// middle text to include in result

		if (join_mode == JoinMode::NEWLINE)
		{
		    ret += UntranslatedText("\n");
		    ret += values[i];
		}
		else
		{
		    // TRANSLATORS: used to construct list of values
		    // %1$s is replaced by first value
		    // %2$s is replaced by second value
		    ret = sformat(_("%1$s, %2$s"), ret, values[i]);
		}
	    }
	    else
	    {
		// last text to include in result

		if (n2 == n1)
		{
		    if (join_mode == JoinMode::NEWLINE)
		    {
			ret += UntranslatedText("\n");
			ret += values[i];
		    }
		    else
		    {
			// TRANSLATORS: used to construct list of values
			// %1$s is replaced by first value
			// %2$s is replaced by second value
			ret = sformat(_("%1$s and %2$s"), ret, values[i]);
		    }
		}
		else
		{
		    if (join_mode == JoinMode::NEWLINE)
		    {
			ret += UntranslatedText("\n");

			// TRANSLATORS: appended after list of value to indicate that some
			// values have been omitted
			// %1$d is replaced by the number of elements omitted
			ret += sformat(_("%1$d more"), n1 - n2 + 1);
		    }
		    else
		    {
			// TRANSLATORS: appended after list of value to indicate that some
			// values have been omitted
			// %1$s is replaced by list of values
			// %2$s is replaced by the number of elements omitted
			ret = sformat(_("%1$s and %2$d more"), ret, n1 - n2 + 1);
		    }
		}
	    }
	}

	return ret;
    }

}
