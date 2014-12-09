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


#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/SystemInfo/ProcParts.h"


namespace storage_bgl
{
    using namespace std;


    ProcParts::ProcParts(bool do_probe)
    {
	if (do_probe)
	    probe();
    }


    void
    ProcParts::probe()
    {
	AsciiFile parts("/proc/partitions");

	parse(parts.lines());
    }


    void
    ProcParts::parse(const vector<string>& lines)
    {
	data.clear();

	for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
	    if (it == lines.begin() || it->empty())
		continue;

	    string device = "/dev/" + extractNthWord(3, *it);
	    unsigned long long sizeK;
	    extractNthWord(2, *it) >> sizeK;
	    data[device] = sizeK;
	}

	y2mil(*this);
    }


    std::ostream& operator<<(std::ostream& s, const ProcParts& procparts)
    {
	for (ProcParts::const_iterator it = procparts.data.begin(); it != procparts.data.end(); ++it)
	    s << "data[" << it->first << "] -> " << it->second << endl;

	return s;
    }


    bool
    ProcParts::findDevice(const string& device) const
    {
    return( findEntry(device)!=data.end() );
    }


    bool
    ProcParts::getSize(const string& device, unsigned long long& sizeK) const
    {
	const_iterator i = findEntry(device);
	if (i == data.end())
	{
	    y2err("dev:" << device << " not found");
	    return false;
	}

	sizeK = i->second;
	y2mil("dev:" << device << " sizeK:" << sizeK);
	return true;
    }


    list<string>
    ProcParts::getEntries() const
    {
	list<string> ret;
	for (const_iterator i = data.begin(); i != data.end(); ++i)
	    ret.push_back(i->first);
	return ret;
    }


    ProcParts::const_iterator
    ProcParts::findEntry(const string& device) const
    {
	return( data.find(normalizeDevice(device)) );
    }

}
