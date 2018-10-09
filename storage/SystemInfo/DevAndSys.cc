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


#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/DevAndSys.h"


namespace storage
{
    using namespace std;


    Dir::Dir(const string& path)
	: path(path)
    {
	SystemCmd cmd(LSBIN " -1 --sort=none " + quote(path));
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("ls failure for " + path));

	parse(cmd.stdout());

	y2mil(*this);
    }


    void
    Dir::parse(const vector<string>& lines)
    {
	entries = lines;
    }


    std::ostream&
    operator<<(std::ostream& s, const Dir& dir)
    {
	s << "path:" << dir.path << " entries:" << dir.entries << '\n';

	return s;
    }


    File::File(const string& path)
	: path(path)
    {
	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	{
	    const Mockup::File& mockup_file = Mockup::get_file(path);
	    content = mockup_file.content;

	    y2mil(*this);
	}

	if (get_remote_callbacks())
	{
	    const RemoteFile mockup_file = get_remote_callbacks()->get_file(path);
	    content = mockup_file.content;
	}
	else
	{
	    ifstream s(path);
	    classic(s);
	    s.unsetf(ifstream::skipws);

	    string line;
	    getline(s, line);
	    while (s.good())
	    {
		content.push_back(line);
		getline(s, line);
	    }
	}

	// TODO error checking

	if (Mockup::get_mode() == Mockup::Mode::RECORD)
	{
	    Mockup::set_file(path, content);
	}

	y2mil(*this);
    }


    template<typename T>
    T
    File::get() const
    {
	if (content.empty())
	    ST_THROW(Exception("empty file " + path));

	T ret;
	content.front() >> ret;

	// TODO error checking

	return ret;
    }


    // specialization using shared one from above
    template int File::get<int>() const;
    template unsigned long long File::get<unsigned long long>() const;
    template unsigned File::get<unsigned>() const;


    template<>
    string
    File::get<string>() const
    {
	if (content.empty())
	    ST_THROW(Exception("empty file " + path));

	return content.front();
    }


    template<>
    bool
    File::get<bool>() const
    {
	return get<int>() != 0;
    }


    std::ostream&
    operator<<(std::ostream& s, const File& file)
    {
	s << "path:" << file.path << " content:" << file.content << '\n';

	return s;
    }


    map<string, string>
    DevLinks::getDirLinks(const string& path) const
    {
	// TODO use cmd(STATBIN " --format '%F %N' " + quote(path) + "/*")?

	SystemCmd cmd(LSBIN " -1l --sort=none " + quote(path));
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("ls failure for " + path));

	return parse(cmd.stdout());
    }


    map<string, string>
    DevLinks::parse(const vector<string>& lines) const
    {
	map<string, string> ret;

	for (const string& line : lines)
	{
	    list<string> tmp = splitString(line);
	    if (tmp.size() >= 3)
	    {
		string v = tmp.back();
		tmp.pop_back();

		if (tmp.back() == "->")
		{
		    tmp.pop_back();

		    string k = tmp.back();

		    ret[k] = v;
		}
	    }
	}

	return ret;
    }


    std::ostream&
    operator<<(std::ostream& s, const DevLinks& devlinks)
    {
	for (const DevLinks::value_type& it : devlinks)
	    s << "data[" << it.first << "] -> " << boost::join(it.second, " ") << '\n';

	return s;
    }


    MdLinks::MdLinks()
    {
        map<string, string> links;
	try
	{
	    links = getDirLinks("/dev/md");
	}
	catch (const Exception&)
	{
	    // OK, no /dev/md at all
	}

	for (const map<string, string>::value_type& it : links)
	{
	    string::size_type pos = it.second.find_first_not_of("./");
	    if (pos != string::npos)
	    {
		data[it.second.substr(pos)].push_back(it.first);
	    }
	}

	y2mil(*this);
    }

}
