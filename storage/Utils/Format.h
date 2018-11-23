/*
 * Copyright (c) 2018 SUSE LLC
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


#ifndef STORAGE_FORMAT_H
#define STORAGE_FORMAT_H


#include <string>
#include <boost/format.hpp>

#include "storage/Utils/Text.h"


namespace storage
{

    using namespace std;


    class SFormatHelper1
    {
    public:

	SFormatHelper1(const string& format)
	    : native(format, locale::classic())
	{
	}

	SFormatHelper1(const char* format)
	    : native(format, locale::classic())
	{
	}

	boost::format native;

	void helper()
	{
	}

	template<typename Type, typename... Args>
	void helper(const Type& value, Args... args)
	{
	    native % value;

	    helper(args...);
	}
    };


    class SFormatHelper2
    {
    public:

	SFormatHelper2(const Text& format)
	    : native(format.native, locale::classic()), translated(format.translated)
	{
	}

	boost::format native;
	boost::format translated;

	void helper()
	{
	}

	template<typename Type, typename... Args>
	void helper(const Type& value, Args... args)
	{
	    native % value;
	    translated % value;

	    helper(args...);
	}

	template<typename... Args>
	void helper(const Text& value, Args... args)
	{
	    native % value.native;
	    translated % value.translated;

	    helper(args...);
	}
    };


    /*
     * The sformat functions here are improved versions of sprintf,
     * esp. they are type safe and accept more types, e.g. string or
     * Text (for sformat taking a Text as format parameter).
     *
     * The sformat functions are implemented using the boost format
     * library so there are differences to the classical printf
     * format. The preferred format string should still be printf
     * style just in case the implementation of sformat should ever
     * change.
     *
     * The native string is not locale aware. This is important since
     * sformat is also used to construct filenames,
     * e.g. sformat("/dev/block/%d:%d", major, minor).
     *
     * Unfortunately the compile-time checks of the format string and
     * the arguments do not work. Neither gcc nor clang does support
     * the required attributes (format and format_arg) for anything
     * else than char* or even variadic templates. That is also the
     * reason why reporting format errors using exceptions is
     * disabled.
     */


    template<typename... Args>
    string
    sformat(const string& format, Args... args)
    {
	// Catching exceptions is required since
	// boost::io::format_string is already raises by the
	// constructor, so disabling exceptions with
	// format::exceptions would happen too late. Using first
	// format::exceptions and then format::parse also does not
	// look so good.

	try
	{
	    SFormatHelper1 sformat_helper(format);

	    sformat_helper.helper(args...);

	    return sformat_helper.native.str();
	}
	catch (const boost::io::format_error&)
	{
	    return format;
	}
    }


    template<typename... Args>
    string
    sformat(const char* format, Args... args)
    {
	try
	{
	    SFormatHelper1 sformat_helper(format);

	    sformat_helper.helper(args...);

	    return sformat_helper.native.str();
	}
	catch (const boost::io::format_error&)
	{
	    return format;
	}
    }


    template<typename... Args>
    Text
    sformat(const Text& format, Args... args)
    {
	try
	{
	    SFormatHelper2 sformat_helper(format);

	    sformat_helper.helper(args...);

	    return Text(sformat_helper.native.str(), sformat_helper.translated.str());
	}
	catch (const boost::io::format_error&)
	{
	    return format;
	}
    }

}


#endif
