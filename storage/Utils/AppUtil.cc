/*
 * Copyright (c) [2004-2014] Novell, Inc.
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
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <string>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/AsciiFile.h"
#include "storage/Storage.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/StorageDefines.h"
#include "storage/Utils/AppUtil.h"
#include "storage/StorageTypes.h"
#include "storage/ArchInfo.h"


namespace storage_bgl
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


    list<string>
    glob(const string& path, int flags)
    {
	list<string> ret;

	glob_t globbuf;
	if (glob(path.c_str(), flags, 0, &globbuf) == 0)
	{
	    for (char** p = globbuf.gl_pathv; *p != 0; p++)
		ret.push_back(*p);
	}
	globfree (&globbuf);

	return ret;
    }


    bool
    getStatVfs(const string& path, StatVfs& buf)
    {
	struct statvfs64 fsbuf;
	if (statvfs64(path.c_str(), &fsbuf) != 0)
	{
	    buf.sizeK = buf.freeK = 0;

	    y2err("errno:" << errno << " " << strerror(errno));
	    return false;
	}

	buf.sizeK = fsbuf.f_blocks;
	buf.sizeK *= fsbuf.f_bsize;
	buf.sizeK /= 1024;

	buf.freeK = fsbuf.f_bfree;
	buf.freeK *= fsbuf.f_bsize;
	buf.freeK /= 1024;

	y2mil("blocks:" << fsbuf.f_blocks << " bfree:" << fsbuf.f_bfree <<
	      " bsize:" << fsbuf.f_bsize << " sizeK:" << buf.sizeK <<
	      " freeK:" << buf.freeK);
	return true;
    }


    bool
    getMajorMinor(const string& device, unsigned long& major, unsigned long& minor, bool may_fail)
    {
	bool ret = false;
	string dev = normalizeDevice(device);
	struct stat sbuf;
	if (stat(device.c_str(), &sbuf) == 0)
	{
	    ret = S_ISBLK(sbuf.st_mode)||S_ISLNK(sbuf.st_mode);
	    if( ret )
		{
		major = gnu_dev_major(sbuf.st_rdev);
		minor = gnu_dev_minor(sbuf.st_rdev);
		}
	}
	else 
	{
	    if( !may_fail )
		y2err("stat for " << device << " failed errno:" << errno << " (" << strerror(errno) << ")");
	}
	return ret;
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


    list<string> normalizeDevices(const list<string>& devs)
    {
	list<string> ret;
	for (list<string>::const_iterator it = devs.begin(); it != devs.end(); ++it)
	    ret.push_back(normalizeDevice(*it));
	return ret;
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

static const string& component = "libstorage";
static string filename;

void createLogger( const string& logpath, const string& logfile )
    {
    filename = logpath + "/" + logfile;
    }

bool queryLog( LogLevel level )
    {
	storage::CallbackLogQuery pfc = storage::getLogQueryCallback();
    return( pfc!=NULL && pfc( level, component ));
    }

bool defaultLogQuery( int level, const string& component )
    {
    return( level != DEBUG );
    }

void
prepareLogStream(ostringstream& stream)
    {
    stream.imbue(std::locale::classic());
    stream.setf(std::ios::boolalpha);
    stream.setf(std::ios::showbase);
    }


ostringstream*
logStreamOpen()
    {
    std::ostringstream* stream = new ostringstream;
    prepareLogStream(*stream);
    return stream;
    }


void
logStreamClose( LogLevel level, const char* file, unsigned line, 
                const char* func, ostringstream* stream )
    {
	storage::CallbackLogDo pfc = storage::getLogDoCallback();
	if (pfc != NULL)
	{
	    string content = stream->str();
	    string::size_type pos1 = 0;
	    while (true)
	    {
		string::size_type pos2 = content.find('\n', pos1);;
		if (pos2 != string::npos || pos1 != content.length())
		    pfc(level, component, file, line, func,
			content.substr(pos1, pos2 - pos1));
		if (pos2 == string::npos)
		    break;
		pos1 = pos2 + 1;
	    }
	}

	delete stream;
    }


static FILE* logf = NULL;

static void close_logf()
    {
    if( logf )
        {
        fclose(logf);
        logf=NULL;
        }
    }
    
void defaultLogDo( int level, const string& comp, const char* file,
                   int line, const char* fct, const string& content )
    {
    ostringstream pfx;
    pfx << datetime(time(0), false, true) << " <" << level << "> "
        << comp << "(" << getpid() << ")" << " " << file 
        << "(" << fct << "):" << line;
    string prefix = pfx.str();

    if( !logf )
        {
        logf = fopen(filename.c_str(), "ae");
        if( logf )
            {
            setlinebuf(logf);
            atexit( close_logf );
            }
        }

    fprintf(logf, "%s - %s\n", prefix.c_str(), content.c_str());
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
	return s + "-part" + decString(num);
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


    bool
    mkdtemp(string& path)
    {
	char* tmp = strdup(path.c_str());
	if (!::mkdtemp(tmp))
	{
	    free(tmp);
	    return false;
	}

	path = tmp;
	free(tmp);
	return true;
    }


bool
readlink(const string& path, string& buf)
{
    char tmp[1024];
    int count = ::readlink(path.c_str(), tmp, sizeof(tmp));
    if (count >= 0)
	buf = string(tmp, count);
    return count != -1;
}


    bool
    readlinkat(int fd, const string& path, string& buf)
    {
        char tmp[1024];
        int count = ::readlinkat(fd, path.c_str(), tmp, sizeof(tmp));
        if (count >= 0)
            buf = string(tmp, count);
        return count != -1;
    }


    list<string>
    getDir(const string& path)
    {
	int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	if (fd < 0)
	{
	    throw runtime_error("open failed for " + path);
	}

	DIR* dir = fdopendir(fd);
	if (dir == NULL)
	{
	    close(fd);
	    throw runtime_error("fdopendir failed for " + path);
	}

	list<string> entries;

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
	    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		continue;

	    entries.push_back(entry->d_name);
	}

	closedir(dir);

	return entries;
    }


    map<string, string>
    getDirLinks(const string& path)
    {
	map<string, string> links;

	int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
	if (fd >= 0)
	{
	    DIR* dir;
	    if ((dir = fdopendir(fd)) != NULL)
	    {
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
		    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		    string tmp;
		    if (readlinkat(fd, entry->d_name, tmp))
			links[entry->d_name] = tmp;
		}
		closedir(dir);
	    }
	    else
	    {
		close(fd);
	    }
	}

	return links;
    }


unsigned
getMajorDevices(const char* driver)
{
    unsigned ret = 0;

    AsciiFile file("/proc/devices");
    const vector<string>& lines = file.lines();

    Regex rx("^" + Regex::ws + "([0-9]+)" + Regex::ws + string(driver) + "$");

    if (find_if(lines, regex_matches(rx)) != lines.end())
	rx.cap(1) >> ret;
    else
	y2err("did not find " << driver << " in /proc/devices");

    y2mil("driver:" << driver << " ret:" << ret);
    return ret;
}


    void
    Text::clear()
    {
	native.clear();
	text.clear();
    }


    const Text&
    Text::operator+=(const Text& a)
    {
	native += a.native;
	text += a.text;
	return *this;
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


    Text
    sformat(const Text& format, ...)
    {
	Text text;
	va_list ap;

	va_start(ap, format);
	text.native = sformat(format.native, ap);
	va_end(ap);

	va_start(ap, format);
	text.text = sformat(format.text, ap);
	va_end(ap);

	return text;
    }


    Text _(const char* msgid)
    {
	return Text(msgid, dgettext("libstorage", msgid));
    }

    Text _(const char* msgid, const char* msgid_plural, unsigned long int n)
    {
	return Text(n == 1 ? msgid : msgid_plural, dngettext("libstorage", msgid, msgid_plural, n));
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


    StopWatch::StopWatch()
    {
	gettimeofday(&start_tv, NULL);
    }


    std::ostream& operator<<(std::ostream& s, const StopWatch& sw)
    {
	struct timeval stop_tv;
	gettimeofday(&stop_tv, NULL);

	struct timeval tv;
	timersub(&stop_tv, &sw.start_tv, &tv);

	return s << fixed << double(tv.tv_sec) + (double)(tv.tv_usec) / 1000000.0 << "s";
    }


    void
    checkBinPaths(const ArchInfo& archinfo, bool instsys)
    {
	y2mil("archinfo:" << archinfo << " instsys:" << instsys);

	set<string> paths = {
#include "../gen_pathlist.cc"
	};

	paths.erase(PORTMAPBIN);
	paths.erase(HFORMATBIN);

	if (!archinfo.is_s390())
	{
	    paths.erase(DASDVIEWBIN);
	    paths.erase(DASDFMTBIN);
	}

	LogLevel level = instsys ? storage_bgl::ERROR : storage_bgl::MILESTONE;
	for (set<string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
        {
            if (access(it->c_str(), X_OK) != 0)
                y2log_op(level, __FILE__, __LINE__, __FUNCTION__,
		    "error accessing " << *it);
	}
    }


const string app_ws = " \t\n";

}
