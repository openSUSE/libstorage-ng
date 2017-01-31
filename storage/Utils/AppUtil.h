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


namespace storage
{
    using std::string;
    using std::vector;
    using std::list;
    using std::map;

    class Arch;


void createPath(const string& Path_Cv);
bool checkNormalFile(const string& Path_Cv);
bool checkDir(const string& Path_Cv);
void checkBinPaths(const Arch& arch, bool instsys);
bool getStatMode(const string& Path_Cv, mode_t& val );
bool setStatMode(const string& Path_Cv, mode_t val );

    string make_dev_block_name(dev_t majorminor);

    std::pair<string, unsigned int> device_to_name_and_number(const string& full_name);
    string name_and_number_to_device(const string& name, unsigned int number);

    vector<string> glob(const string& path, int flags);

    struct StatVfs
    {
	unsigned long long size;
	unsigned long long free;
    };

    StatVfs detect_stat_vfs(const string& path);


string extractNthWord(int Num_iv, const string& Line_Cv, bool GetRest_bi = false);
std::list<string> splitString( const string& s, const string& delChars=" \t\n",
                          bool multipleDelim=true, bool skipEmpty=true,
			  const string& quotes="" );
std::map<string,string> makeMap( const std::list<string>& l,
                                 const string& delim = "=",
				 const string& removeSur = " \t\n" );

    string udevAppendPart(const string&, unsigned num);
    string afterLast(const string& s, const string& pat );

    string udevEncode(const string&);
    string udevDecode(const string&);

    string normalizeDevice(const string& dev);
    string undevDevice(const string& dev);

// TODO move to NfsImpl.h
bool isNfsDev( const string& dev );


template<class StreamType>
void classic(StreamType& stream)
{
    stream.imbue(std::locale::classic());
}


    string hostname();
    string datetime( time_t t1, bool utc=false, bool classic=true );


    string sformat(const string& format, va_list ap);

    string sformat(const string& format, ...);


extern const string app_ws;

}

#endif
