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


#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/io/ios_state.hpp>

#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/SystemInfo/Arch.h"
#include "storage/Utils/LoggerImpl.h"


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
getStatMode(const string& Path_Cv, mode_t& val )
{
  struct stat Stat_ri;
  int ret_ii = stat(Path_Cv.c_str(), &Stat_ri);

  if( ret_ii==0 )
    val = Stat_ri.st_mode;
  else
    y2mil( "stat " << Path_Cv << " ret:" << ret_ii );

  return (ret_ii==0);
}

bool
setStatMode(const string& Path_Cv, mode_t val )
{
  int ret_ii = chmod( Path_Cv.c_str(), val );
  if( ret_ii!=0 )
    y2mil( "chmod " << Path_Cv << " ret:" << ret_ii );
  return( ret_ii==0 );
}


bool
checkNormalFile(const string& Path_Cv)
{
  struct stat Stat_ri;

  return (stat(Path_Cv.c_str(), &Stat_ri) >= 0 &&
	  S_ISREG(Stat_ri.st_mode));
}


    string
    make_dev_block_name(dev_t majorminor)
    {
	return sformat(DEVDIR "/block/%d:%d", gnu_dev_major(majorminor),
		       gnu_dev_minor(majorminor));
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
	if (!boost::starts_with(dev, "/dev/") && !isNfsDev(dev))
	    return "/dev/" + dev;
	return dev;
    }


bool isNfsDev( const string& dev )
    {
    return( !dev.empty() && dev[0]!='/' &&
            dev.find( ':' )!=string::npos );
    }


    string undevDevice(const string& dev)
    {
	if (boost::starts_with(dev, "/dev/"))
	    return string(dev, 5);
	return dev;
    }


string afterLast(const string& s, const string& pat )
    {
    string ret(s);
    string::size_type pos = s.find_last_of(pat);
    if( pos!=string::npos )
	ret.erase( 0, pos+pat.length() );
    return( ret );
    }

    string
    udevAppendPart(const string& s, unsigned num)
    {
	return s + "-part" + to_string(num);
    }


    string
    udevEncode(const string& s)
    {
	string r = s;

	string::size_type pos = 0;

	while (true)
	{
	    pos = r.find_first_of(" '\\/", pos);
	    if (pos == string::npos)
		break;

	    char tmp[16];
	    sprintf(tmp, "\\x%02x", r[pos]);
	    r.replace(pos, 1, tmp);

	    pos += 4;
	}

	return r;
    }


    string
    udevDecode(const string& s)
    {
	string r = s;

	string::size_type pos = 0;

	while (true)
	{
	    pos = r.find("\\x", pos);
	    if (pos == string::npos || pos > r.size() - 4)
		break;

	    unsigned int tmp;
	    if (sscanf(r.substr(pos + 2, 2).c_str(), "%x", &tmp) == 1)
		r.replace(pos, 4, 1, (char) tmp);

	    pos += 1;
	}

	return r;
    }


    string
    sformat(const string& format, va_list ap)
    {
	char* result;

	if (vasprintf(&result, format.c_str(), ap) == -1)
	    return string();

	string str(result);
	free(result);
	return str;
    }


    string
    sformat(const string& format, ...)
    {
	va_list ap;

	va_start(ap, format);
	string s = sformat(format, ap);
	va_end(ap);

	return s;
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
    datetime( time_t t1, bool utc, bool classic )
    {
	struct tm t2;
	utc ? gmtime_r(&t1, &t2) : localtime_r(&t1, &t2);
	char buf[64 + 1];
	if (strftime(buf, sizeof(buf), classic ? "%F %T" : "%c", &t2) == 0)
	    return string("unknown");
	return string(buf);
    }


    void
    checkBinPaths(const Arch& arch, bool instsys)
    {
	y2mil("arch:" << arch << " instsys:" << instsys);

	set<string> paths = {
#include "gen-pathlist.cc"
	};

	paths.erase(PORTMAPBIN);
	paths.erase(HFORMATBIN);

	if (!arch.is_s390())
	{
	    paths.erase(DASDVIEWBIN);
	    paths.erase(DASDFMTBIN);
	}

	LogLevel log_level = instsys ? storage::ERROR : storage::MILESTONE;
	for (const string& name : paths)
        {
            if (access(name.c_str(), X_OK) != 0)
                y2log_op(log_level, __FILE__, __LINE__, __FUNCTION__, "error accessing " <<
			 name);
	}
    }


const string app_ws = " \t\n";

    /**
     * Breaks a device name like "/dev/sda2" into "/dev/sda" and 2
     *
     * Throws an exception if the provided name does not contain the number
     * part.
     */
    std::pair<string, unsigned int>
    device_to_name_and_number(const string& full_name)
    {
	string::size_type pos = full_name.find_last_not_of("0123456789");
        if (pos == string::npos || pos == full_name.size() - 1)
            ST_THROW(Exception("device name has no number"));

	return std::make_pair(full_name.substr(0, pos + 1), atoi(full_name.substr(pos + 1).c_str()));
    }

    /**
     * Inverse of device_to_name_and_number
     */
    string
    name_and_number_to_device(const string& name, unsigned int number)
    {
	return (name + to_string(number));
    }
}
