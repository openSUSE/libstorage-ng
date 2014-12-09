/*
 * Copyright (c) [2004-2010] Novell, Inc.
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


#include <glob.h>

#include "storage/FreeInfo.h"
#include "storage/StorageDefines.h"
#include "storage/Volume.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    FreeInfo::FreeInfo(const xmlNode* node)
	: resize_cached(false), content_cached(false)
    {
	if (getChildValue(node, "resize_cached", resize_cached) && resize_cached)
	{
	    getChildValue(node, "df_free_k", resize_info.df_freeK);
	    getChildValue(node, "resize_free_k", resize_info.resize_freeK);
	    getChildValue(node, "used_k", resize_info.usedK);
	    getChildValue(node, "resize_ok", resize_info.resize_ok);
	}

	if (getChildValue(node, "content_cached", content_cached) && content_cached)
	{
	    getChildValue(node, "windows", content_info.windows);
	    getChildValue(node, "efi", content_info.efi);
	    getChildValue(node, "homes", content_info.homes);
	}
    }


    void
    FreeInfo::saveData(xmlNode* node) const
    {
	if (resize_cached)
	{
	    setChildValue(node, "resize_cached", resize_cached);

	    setChildValue(node, "df_free_k", resize_info.df_freeK);
	    setChildValue(node, "resize_free_k", resize_info.resize_freeK);
	    setChildValue(node, "used_k", resize_info.usedK);
	    setChildValue(node, "resize_ok", resize_info.resize_ok);
	}

	if (content_cached)
	{
	    setChildValue(node, "content_cached", content_cached);

	    setChildValue(node, "windows", content_info.windows);
	    setChildValue(node, "efi", content_info.efi);
	    setChildValue(node, "homes", content_info.homes);
	}
    }


    void
    FreeInfo::update(bool new_resize_cached, const ResizeInfo& new_resize_info,
		     bool new_content_cached, const ContentInfo& new_content_info)
    {
	if (new_resize_cached)
	{
	    resize_cached = true;
	    resize_info = new_resize_info;
	}

	if (new_content_cached)
	{
	    content_cached = true;
	    content_info = new_content_info;
	}
    }


    bool
    FreeInfo::isWindows(const string& mp)
    {
	const char* files[] = { "boot.ini", "msdos.sys", "io.sys", "config.sys", "MSDOS.SYS",
				"IO.SYS", "bootmgr", "$Boot" };

	for (unsigned int i = 0; i < lengthof(files); ++i)
	{
	    string file = mp + "/" + files[i];
	    if (access(file.c_str(), R_OK) == 0)
	    {
		y2mil("found windows file " << quote(file));
		return true;
	    }
	}

	return false;
    }


    unsigned
    FreeInfo::numHomes(const string& mp)
    {
	const char* files[] = { ".profile", ".bashrc", ".ssh", ".kde", ".kde4", ".gnome",
				".gnome2" };

	unsigned num = 0;

	const list<string> dirs = glob(mp + "/*", GLOB_NOSORT | GLOB_ONLYDIR);
	for (list<string>::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
	{
	    if ( !boost::ends_with(*dir,"/root") && checkDir(*dir))
	    {
		for (unsigned int i = 0; i < lengthof(files); ++i)
		{
		    string file = *dir + "/" + files[i];
		    if (access(file.c_str(), R_OK) == 0)
		    {
			y2mil("found home file " << quote(file));
			++num;
			break;
		    }
		}
	    }

	    if (num >= 2)
		return num;
	}

	return num;
    }


    std::ostream& operator<<(std::ostream& s, const ResizeInfo& resize_info)
    {
	return s << "df_freeK:" << resize_info.df_freeK << " resize_freeK:" << resize_info.resize_freeK
		 << " usedK:" << resize_info.usedK << " resize_ok:" << resize_info.resize_ok;
    }


    std::ostream& operator<<(std::ostream& s, const ContentInfo& content_info)
    {
	return s << "windows:" << content_info.windows << " efi:" << content_info.efi
		 << " homes:" << content_info.homes;
    }


    std::ostream& operator<<(std::ostream& s, const FreeInfo& free_info)
    {
	s << "resize_cached:" << free_info.resize_cached;
	if (free_info.resize_cached)
	    s << " resize_info " << free_info.resize_info;

	s << "content_cached:" << free_info.content_cached;
	if (free_info.content_cached)
	    s << " content_info " << free_info.content_info;

	return s;
    }


    ResizeInfo
    FreeInfo::detectResizeInfo(const string& mp, const Volume& vol)
    {
	ResizeInfo resize_info;

	StatVfs vfsbuf;
	bool ret = getStatVfs(mp, vfsbuf);
	if (ret)
	{
	    resize_info.df_freeK = vfsbuf.freeK;
	    resize_info.usedK = vfsbuf.sizeK - vfsbuf.freeK;
	    resize_info.resize_freeK = resize_info.df_freeK;
	    resize_info.resize_ok = true;
	}

	if (ret && vol.getFs() == NTFS)
	{
	    SystemCmd c( NTFSRESIZEBIN " -f -i " + quote(vol.device()));
	    string fstr = " might resize at ";
	    string::size_type pos;
	    string stdout = boost::join(c.stdout(), "\n");
	    if (c.retcode()==0 && (pos=stdout.find(fstr))!=string::npos)
	    {
		y2mil("pos:" << pos);
		pos = stdout.find_first_not_of(" \t\n", pos + fstr.size());
		y2mil("pos:" << pos);
		string number = stdout.substr(pos, stdout.find_first_not_of("0123456789", pos));
		y2mil("number:\"" << number << "\"");
		unsigned long long t;
		number >> t;
		y2mil("number:" << t);
		if (t - vol.sizeK() < resize_info.resize_freeK)
		    resize_info.resize_freeK = t - vol.sizeK();
	    }
	    else
	    {
		resize_info.resize_ok = false;
	    }
	}

	y2mil("device:" << vol.device() << " " << resize_info);
	return resize_info;
    }


    ContentInfo
    FreeInfo::detectContentInfo(const string& mp, const Volume& vol)
    {
	ContentInfo content_info;

	content_info.efi = vol.getFs() == VFAT && checkDir(mp + "/efi");

	if (!content_info.efi && (vol.getFs() == VFAT || vol.getFs() == NTFS))
	    content_info.windows = isWindows(mp);

	content_info.homes = numHomes(mp);

	if( content_info.homes==0 && !vol.getLabel().empty() )
	    {
	    string lab = boost::to_lower_copy(vol.getLabel());
	    y2mil( "label:" << vol.getLabel() << " lab:" << lab );
	    if( boost::starts_with( lab, "home" ))
		content_info.homes = 1;
	    }
	if( content_info.homes==0 && 
	    (vol.getFs()==EXT2 || vol.getFs()==EXT3 || vol.getFs()==EXT4 ))
	    {
	    SystemCmd c( TUNE2FSBIN " -l " + vol.mountDevice() + " | " GREPBIN " '^Last mounted on:'" );
	    if( c.retcode()==0 && c.numLines()>0 )
		{
		string line = c.getLine(0);
		line = line.substr( line.find( ':' )+1 );
		y2mil( "line:" << c.getLine(0) << " lin:" << line );
		line = extractNthWord( 0, line );
		y2mil( "dir:" << line );
		if( line == "/home" )
		    content_info.homes = 1;
		}
	    }
	y2mil("device:" << vol.device() << " " << content_info);
	return content_info;
    }

}
