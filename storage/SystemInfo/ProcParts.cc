/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) 2017 SUSE LLC
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


#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/SystemInfo/ProcParts.h"


namespace storage
{
    using namespace std;


    ProcParts::ProcParts()
    {
	AsciiFile parts("/proc/partitions");

	parse(parts.get_lines());
    }


    void
    ProcParts::parse(const vector<string>& lines)
    {
	data.clear();

	for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
	    if (it == lines.begin() || it->empty())
		continue;

	    string device = DEVDIR "/" + extractNthWord(3, *it);
	    unsigned long long size_k;
	    extractNthWord(2, *it) >> size_k;
	    data[device] = size_k * KiB;
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const ProcParts& proc_parts)
    {
	for (const pair<string, unsigned long long>& entry : proc_parts)
	    s << "data[" << entry.first << "] -> " << entry.second << '\n';

	return s;
    }


    bool
    ProcParts::find_device(const string& device) const
    {
	return find_entry(device) != data.end();
    }


    bool
    ProcParts::get_size(const string& device, unsigned long long& size) const
    {
	const_iterator i = find_entry(device);
	if (i == data.end())
	{
	    y2err("dev:" << device << " not found");
	    return false;
	}

	size = i->second;
	y2mil("dev:" << device << " size:" << size);
	return true;
    }


    vector<string>
    ProcParts::get_entries() const
    {
	vector<string> ret;
	for (const_iterator i = data.begin(); i != data.end(); ++i)
	    ret.push_back(i->first);
	return ret;
    }


    ProcParts::const_iterator
    ProcParts::find_entry(const string& device) const
    {
	return data.find(normalizeDevice(device));
    }

}
