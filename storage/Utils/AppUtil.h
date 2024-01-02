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


#ifndef STORAGE_APP_UTIL_H
#define STORAGE_APP_UTIL_H


#include <sys/time.h>
#include <sys/types.h>
#include <sstream>
#include <locale>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <regex>
#include <utility>

#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::list;
    using std::map;
    using std::regex;
    using std::pair;

    class Arch;


    void createPath(const string& Path_Cv);
    bool checkNormalFile(const string& Path_Cv);
    bool checkDir(const string& Path_Cv);

    string dirname(const string& name);
    string basename(const string& name);

    string make_dev_block_name(dev_t majorminor);

    pair<string, unsigned int> device_to_name_and_number(const string& full_name);
    string name_and_number_to_device(const string& name, unsigned int number);

    vector<string> glob(const string& path, int flags);

    struct StatVfs
    {
	unsigned long long size;
	unsigned long long free;
    };

    StatVfs detect_stat_vfs(const string& path);

    /**
     * Check if the block device has holders as reported by the kernel in
     * /sys. If the block device is a partition the disk is also checked for
     * holders.
     */
    bool has_kernel_holders(const string& name, SystemInfo::Impl& system_info);

    string extractNthWord(int Num_iv, const string& Line_Cv, bool GetRest_bi = false);

    std::list<string> splitString( const string& s, const string& delChars=" \t\n",
                                   bool multipleDelim=true, bool skipEmpty=true,
                                   const string& quotes="" );

    std::map<string,string> makeMap( const std::list<string>& l,
                                     const string& delim = "=",
                                     const string& removeSur = " \t\n" );

    string udev_encode(const string&);
    string udev_decode(const string&);

    string normalizeDevice(const string& dev);


    template<class StreamType>
    void classic(StreamType& stream)
    {
        stream.imbue(std::locale::classic());
    }


    string stringerror(int errnum);

    string hostname();
    string datetime(time_t t1, bool utc = true);


    /**
     * Return a string with the time in various forms, e.g. uptime and UTC, to
     * allow easy synchronisation between different logs, e.g. dmesg.
     */
    string timestamp();

    string generated_string();


    /**
     * Return a string which used in a regex matches itself and nothing else.
     *
     * So far only implemented for regex::extended.
     */
    string
    regex_escape(const string& s, regex::flag_type f);


    /**
     * Pads the string s to at least width w with char c at the front.
     */
    string
    pad_front(const string& s, size_t w, char c);


    /**
     * Padding information for NameSchema.
     */
    struct PadInfo
    {
	enum Method
	{
	    /**
	     * Numeric pad. The number is padded with zeros.
	     */
	    N1,

	    /**
	     * Alphabetical pad. The string (must only contain 'a' to
	     * 'z') is transformed to a number and that number is
	     * padded with zeros.
	     *
	     * This method should be locale invariant. E.g. simply
	     * padding the string with spaces is not locale invariant.
	     */
	    A1
	};

	PadInfo(Method method, size_t size) : method(method), size(size) {}

	const Method method;
	const size_t size;
    };


    /**
     * Definition of a name schema used by format_to_name_schemata().
     */
    struct NameSchema
    {
	NameSchema(regex re, const vector<PadInfo>& pad_infos)
	    : re(re), pad_infos(pad_infos) {}

	/**
	 * Regular expression matching the name schema.
	 */
	const regex re;

	/**
	 * Width and char to which the sub-matches of the regular
	 * expression will be padded.
	 */
	const vector<PadInfo> pad_infos;
    };


    /**
     * Formats the string s to the first matching name schema: All sub-matches
     * will be padded as defined in the name schema.
     *
     * This can be used to sort device names. E.g. if it is desired to
     * have "sdb" before "sdaa" the names can be transformed to "sd b"
     * and "sdaa" and be sorted alphabetical. Used by
     * Device::get_name_sort_key().
     */
    string
    format_to_name_schemata(const string& s, const vector<NameSchema>& name_schemata);


    extern const string app_ws;

}

#endif
