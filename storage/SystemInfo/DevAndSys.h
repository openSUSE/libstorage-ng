/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#ifndef STORAGE_DEV_AND_SYS_H
#define STORAGE_DEV_AND_SYS_H


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <map>

#include "storage/Utils/Udev.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


    /**
     * A sequence of file names found in a pathname.
     */
    class Dir
    {
    public:

	Dir(Udevadm& udevadm, const string& path);

	typedef vector<string>::const_iterator const_iterator;

	bool empty() const { return entries.empty(); }

	const_iterator begin() const { return entries.begin(); }
	const_iterator end() const { return entries.end(); }

	friend std::ostream& operator<<(std::ostream& s, const Dir& dir);

    private:

	void parse(const vector<string>& lines);

	const string path;

	vector<string> entries;

    };


    /**
     * A sequence of lines, constructed from a pathname.
     *
     * Trailing newlines are not included.
     */
    class File
    {
    public:

	typedef vector<string>::const_iterator const_iterator;

	File(const string& path);

	const_iterator begin() const { return content.begin(); }
	const_iterator end() const { return content.end(); }

	friend std::ostream& operator<<(std::ostream& s, const File& file);

	template<typename Type> Type get() const;

    private:

	const string path;

	vector<string> content;

    };


    class DevLinks
    {
    public:

	typedef map<string, vector<string>>::value_type value_type;
	typedef map<string, vector<string>>::const_iterator const_iterator;

	const_iterator begin() const { return data.begin(); }
	const_iterator end() const { return data.end(); }

	const_iterator find(const string& nm) const { return data.find(nm); }

	friend std::ostream& operator<<(std::ostream& s, const DevLinks& devlinks);

    protected:

	map<string, string> getDirLinks(Udevadm& udevadm, const string& path) const;
	map<string, string> parse(const vector<string>& lines) const;

	map<string, vector<string>> data;

    };


    class MdLinks : public DevLinks
    {
    public:

	MdLinks(Udevadm& udevadm);

    };

}

#endif
