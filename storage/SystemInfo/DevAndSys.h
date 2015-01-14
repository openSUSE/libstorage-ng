/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#ifndef DEV_AND_SYS_H
#define DEV_AND_SYS_H


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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


    class MajorMinor
    {
    public:

	MajorMinor(const string& device);

	friend std::ostream& operator<<(std::ostream& s, const MajorMinor& majorminor);

	dev_t getMajorMinor() const { return majorminor; }
	unsigned int getMajor() const { return gnu_dev_major(majorminor); }
	unsigned int getMinor() const { return gnu_dev_minor(majorminor); }

    private:

	void parse(const vector<string>& lines);

	string device;
	dev_t majorminor;

    };


    class Dir
    {
    public:

	Dir(const string& path);

	typedef list<string>::const_iterator const_iterator;

	const_iterator begin() const { return entries.begin(); }
	const_iterator end() const { return entries.end(); }

	friend std::ostream& operator<<(std::ostream& s, const Dir& dir);

    private:

	void parse(const vector<string>& lines);

	string path;
	list<string> entries;

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

	map<string, string> getDirLinks(const string& path) const;
	map<string, string> parse(const vector<string>& lines) const;

	map<string, vector<string>> data;

    };


    class MdLinks : public DevLinks
    {
    public:

	MdLinks();

    };

}

#endif
