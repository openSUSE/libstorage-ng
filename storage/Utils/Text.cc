/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
#include <stdarg.h>

#include "storage/Utils/Text.h"
#include "storage/Utils/AppUtil.h"


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
    sformat(const Text& format, ...)
    {
	Text text;
	va_list ap;

	va_start(ap, format);
	text.native = sformat(format.native, ap);
	va_end(ap);

	va_start(ap, format);
	text.translated = sformat(format.translated, ap);
	va_end(ap);

	return text;
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

}
