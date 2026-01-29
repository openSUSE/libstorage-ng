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


#include <unistd.h>
#include <fcntl.h>
#include <glob.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <string>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/AppUtil.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{
    using namespace std;


    void createPath(const string& Path_Cv)
    {
        string::size_type Pos_ii = 0;
        while ((Pos_ii = Path_Cv.find('/', Pos_ii + 1)) != string::npos)
        {
            string Tmp_Ci = Path_Cv.substr(0, Pos_ii);
            mkdir(Tmp_Ci.c_str(), 0777);
        }
        mkdir(Path_Cv.c_str(), 0777);
    }


    bool
    checkDir(const string& Path_Cv)
    {
        struct stat Stat_ri;

        return (stat(Path_Cv.c_str(), &Stat_ri) >= 0 &&
                S_ISDIR(Stat_ri.st_mode));
    }


    bool
    checkNormalFile(const string& Path_Cv)
    {
        struct stat Stat_ri;

        return (stat(Path_Cv.c_str(), &Stat_ri) >= 0 &&
                S_ISREG(Stat_ri.st_mode));
    }


    string
    dirname(const string& name)
    {
	string::size_type pos = name.find_last_of('/');
	if (pos == string::npos)
	    return string(".");
	return string(name, 0, pos == 0 ? 1 : pos);
    }


    string
    basename(const string& name)
    {
	string::size_type pos = name.find_last_of('/');
	return string(name, pos + 1);
    }


    string
    make_dev_block_name(dev_t majorminor)
    {
	return sformat(DEV_DIR "/block/%d:%d", major(majorminor), minor(majorminor));
    }


    vector<string>
    glob(const string& path, int flags)
    {
	vector<string> ret;

	glob_t globbuf;
	if (glob(path.c_str(), flags, 0, &globbuf) == 0)
	{
	    for (char** p = globbuf.gl_pathv; *p != 0; p++)
		ret.push_back(*p);
	}
	globfree (&globbuf);

	return ret;
    }


    StatVfs
    detect_stat_vfs(const string& path)
    {
	struct statvfs64 fsbuf;
	if (statvfs64(path.c_str(), &fsbuf) != 0)
	{
	    ST_THROW(Exception("statvfs64 failed"));
	}

	StatVfs stat_vfs;

	stat_vfs.size = fsbuf.f_blocks;
	stat_vfs.size *= fsbuf.f_bsize;

	stat_vfs.free = fsbuf.f_bfree;
	stat_vfs.free *= fsbuf.f_bsize;

	y2mil("path:" << path << " blocks:" << fsbuf.f_blocks << " bfree:" << fsbuf.f_bfree
	      << " bsize:" << fsbuf.f_bsize << " size:" << stat_vfs.size
	      << " free:" << stat_vfs.free);

	return stat_vfs;
    }


    bool
    has_kernel_holders(const string& name, SystemInfo::Impl& system_info)
    {
	const CmdUdevadmInfo& cmd_udevadm_info = system_info.getCmdUdevadmInfo(name);

	string sysfs_path = cmd_udevadm_info.get_path();

	if (!system_info.getDir(SYSFS_DIR + sysfs_path + "/holders").empty())
	    return true;

	if (cmd_udevadm_info.get_device_type() == CmdUdevadmInfo::DeviceType::PARTITION)
	{
	    sysfs_path = dirname(sysfs_path);

	    if (!system_info.getDir(SYSFS_DIR + sysfs_path + "/holders").empty())
		return true;
	}

	return false;
    }


    string extractNthWord(int Num_iv, const string& Line_Cv, bool GetRest_bi)
    {
        string::size_type pos;
        int I_ii=0;
        string Ret_Ci = Line_Cv;

        if( Ret_Ci.find_first_of(app_ws)==0 )
        {
            pos = Ret_Ci.find_first_not_of(app_ws);
            if( pos != string::npos )
            {
                Ret_Ci.erase(0, pos );
            }
            else
            {
                Ret_Ci.erase();
            }
        }
        while( I_ii<Num_iv && Ret_Ci.length()>0 )
        {
            pos = Ret_Ci.find_first_of(app_ws);
            if( pos != string::npos )
            {
                Ret_Ci.erase(0, pos );
            }
            else
            {
                Ret_Ci.erase();
            }
            if( Ret_Ci.find_first_of(app_ws)==0 )
            {
                pos = Ret_Ci.find_first_not_of(app_ws);
                if( pos != string::npos )
                {
                    Ret_Ci.erase(0, pos );
                }
                else
                {
                    Ret_Ci.erase();
                }
            }
            I_ii++;
        }
        if (!GetRest_bi && (pos=Ret_Ci.find_first_of(app_ws))!=string::npos )
            Ret_Ci.erase(pos);
        return Ret_Ci;
    }


    list<string> splitString( const string& s, const string& delChars,
                              bool multipleDelim, bool skipEmpty,
                              const string& quotes )
    {
        string::size_type pos;
        string::size_type cur = 0;
        string::size_type nfind = 0;
        list<string> ret;

        while( cur<s.size() && (pos=s.find_first_of(delChars,nfind))!=string::npos )
        {
            if( pos==cur )
            {
                if( !skipEmpty )
                    ret.push_back( "" );
            }
            else
                ret.push_back( s.substr( cur, pos-cur ));
            if( multipleDelim )
            {
                cur = s.find_first_not_of(delChars,pos);
            }
            else
                cur = pos+1;
            nfind = cur;
            if( !quotes.empty() )
            {
                string::size_type qpos=s.find_first_of(quotes,cur);
                string::size_type lpos=s.find_first_of(delChars,cur);
                if( qpos!=string::npos && qpos<lpos &&
                    (qpos=s.find_first_of(quotes,qpos+1))!=string::npos )
                {
                    nfind = qpos;
                }
            }
        }
        if( cur<s.size() )
            ret.push_back( s.substr( cur ));
        if( !skipEmpty && !s.empty() && s.find_last_of(delChars)==s.size()-1 )
            ret.push_back( "" );
        //y2mil( "ret:" << ret );
        return( ret );
    }


    map<string,string>
    makeMap( const list<string>& l, const string& delim, const string& removeSur )
    {
        map<string,string> ret;
        for( list<string>::const_iterator i=l.begin(); i!=l.end(); ++i )
        {
            string k, v;
            string::size_type pos;
            if( (pos=i->find_first_of( delim ))!=string::npos )
            {
                k = i->substr( 0, pos );
                string::size_type pos2 = i->find_first_not_of( delim, pos+1 );
                if( pos2 != string::npos )
                    v = i->substr( pos2 );
            }
            if( !removeSur.empty() )
            {
                if( (pos=k.find_first_of(removeSur)) != string::npos )
                    k.erase( 0, k.find_first_not_of(removeSur) );
                if( !k.empty() && (pos=k.find_last_of(removeSur))==k.size()-1 )
                    k.erase( k.find_last_not_of(removeSur)+1 );
                if( (pos=v.find_first_of(removeSur)) != string::npos )
                    v.erase( 0, v.find_first_not_of(removeSur) );
                if( !v.empty() && (pos=v.find_last_of(removeSur))==v.size()-1 )
                    v.erase( v.find_last_not_of(removeSur)+1 );
            }
            if( !k.empty() && !v.empty() )
                ret[k] = v;
        }
        return( ret );
    }


    string normalizeDevice(const string& dev)
    {
	if (!boost::starts_with(dev, "/dev/"))
	    return "/dev/" + dev;
	return dev;
    }


    string
    stringerror(int errnum)
    {
#if (_POSIX_C_SOURCE >= 200112L) && ! _GNU_SOURCE
	char buf1[100];
	if (strerror_r(errnum, buf1, sizeof(buf1) - 1) == 0)
	    return string(buf1);
	return string("strerror_r failed");
#else
	char buf1[100];
	const char* buf2 = strerror_r(errnum, buf1, sizeof(buf1) - 1);
	return string(buf2);
#endif
    }


    string
    hostname()
    {
	struct utsname buf;
	if (uname(&buf) != 0)
	    return string("unknown");
	string hostname(buf.nodename);
	if (strlen(buf.domainname) > 0)
	    hostname += "." + string(buf.domainname);
	return hostname;
    }


    string
    datetime(time_t t1, bool utc)
    {
	struct tm t2;
	utc ? gmtime_r(&t1, &t2) : localtime_r(&t1, &t2);
	char buf[64 + 1];
	if (strftime(buf, sizeof(buf), "%F %T %Z", &t2) == 0)
	    return string("unknown");
	return string(buf);
    }


    string
    timestamp()
    {
	// CLOCK_MONOTONIC seems to be the time used in dmesg. /proc/uptime,
	// sysinfo(2) and CLOCK_BOOTTIME are different when it comes to
	// handling suspend.

	struct timespec tmp1;
	clock_gettime(CLOCK_MONOTONIC, &tmp1);
	double t1 = tmp1.tv_sec + 1.0e-9 * tmp1.tv_nsec;

	time_t t2 = time(0);

	return "[" + to_string(t1) + "], " + datetime(t2, true) + ", " + datetime(t2, false);
    }


    string
    generated_string()
    {
	return "generated by libstorage-ng version " VERSION ", " + hostname() + ", " +
	    datetime(time(nullptr));
    }


    const string app_ws = " \t\n";


    /**
     * Breaks a device name like "/dev/sda2" into "/dev/sda" and 2.
     *
     * Does in general not break partition names into disk name and number,
     * e.g. "/dev/md0p1" breaks into "/dev/md0p" and "1".
     *
     * Throws an exception if the provided name does not contain the number
     * part.
     */
    pair<string, unsigned int>
    device_to_name_and_number(const string& full_name)
    {
	string::size_type pos = full_name.find_last_not_of("0123456789");
	if (pos == string::npos || pos == full_name.size() - 1)
	    ST_THROW(Exception("device name has no number"));

	return make_pair(full_name.substr(0, pos + 1), atoi(full_name.substr(pos + 1).c_str()));
    }


    /**
     * Inverse of device_to_name_and_number.
     */
    string
    name_and_number_to_device(const string& name, unsigned int number)
    {
	return name + to_string(number);
    }


    string
    regex_escape(const string& s, regex::flag_type f)
    {
	// If the function is needed for any other flags it will have to be checked or
	// even extended.
	if (f != regex::extended)
	    ST_THROW(Exception("regex_escape does not support used flags"));

	// All special characters outside of any context, see
	// https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_04
	// (linked from https://en.cppreference.com/w/cpp/regex/basic_regex). Note that
	// escaping more characters leads to problems with -std=c++14 but not with -std=gnu++14.
	static const string special_chars(".^$|[(){*+?\\");

	string r;

	for (char c : s)
	{
	    if (special_chars.find(c) != string::npos)
		r += "\\";
	    r += c;
	}

	return r;
    }


    string
    pad_front(const string& s, size_t w, char c)
    {
	if (s.size() >= w)
	    return s;

	return string(w - s.size(), c) + s;
    }


    /**
     * Enumerates string containing 'a' to 'z'. "a" is 1, "z" is 26, "aa" is 27 and so on.
     */
    unsigned int
    enumerate_strings(const string& s)
    {
	unsigned int i = 0;

	for (char c : s)
	    i = (26 * i) + (c - 'a' + 1);

	return i;
    }


    string
    format_to_name_schemata(const string& s, const vector<NameSchema>& name_schemata)
    {
	for (const NameSchema& name_schema : name_schemata)
	{
	    smatch match;

	    if (regex_match(s, match, name_schema.re) && match.size() - 1 == name_schema.pad_infos.size())
	    {
		string ret = s;

		for (size_t i = match.size() - 1; i > 0; --i)
		{
		    const PadInfo& pad_info = name_schema.pad_infos[i - 1];

		    string replacement;

		    switch (pad_info.method)
		    {
			case PadInfo::N1:
			    replacement = pad_front(match.str(i), pad_info.size, '0');
			    break;

			case PadInfo::A1:
			    replacement = pad_front(to_string(enumerate_strings(match.str(i))), pad_info.size, '0');
			    break;
		    }

		    ret.replace(match.position(i), match.length(i), replacement);
		}

		return ret;
	    }
	}

	return s;
    }

}
