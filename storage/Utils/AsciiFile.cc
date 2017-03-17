/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#include <unistd.h>
#include <fstream>

#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageTypes.h"


namespace storage
{
    using namespace std;


    AsciiFile::AsciiFile(const string& name, bool remove_empty)
	: name(name), remove_empty(remove_empty)
    {
	reload();
    }


    bool
    AsciiFile::reload()
    {
	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	{
	    const Mockup::File& mockup_file = Mockup::get_file(name);
	    lines = mockup_file.content;
	    return true;
	}

	bool ret;

	if (get_remote_callbacks())
	{
	    const RemoteFile remote_file = get_remote_callbacks()->get_file(name);
	    lines = remote_file.content;
	    ret = true;
	}
	else
	{
	    y2mil("loading file " << name);
	    clear();

	    ifstream file(name);
	    classic(file);
	    string line;

	    ret = file.good();
	    file.unsetf(ifstream::skipws);
	    getline(file, line);
	    while (file.good())
	    {
		lines.push_back(line);
		getline(file, line);
	    }
	}

	if (Mockup::get_mode() == Mockup::Mode::RECORD)
	{
	    Mockup::set_file(name, lines);
	}

	return ret;
    }


    bool
    AsciiFile::save()
    {
	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	{
	    Mockup::set_file(name, lines);
	    return true;
	}

	if (Mockup::get_mode() == Mockup::Mode::RECORD)
	{
	    Mockup::set_file(name, lines);
	}

	if (remove_empty && empty())
	{
	    y2mil("deleting file " << name);

	    if (access(name.c_str(), F_OK) != 0)
		return true;

	    return unlink(name.c_str()) == 0;
	}
	else
	{
	    y2mil("saving file " << name);

	    ofstream file(name);
	    classic(file);

	    for (const string& line : lines)
	    	file << line << std::endl;

	    file.close();

	    return file.good();
	}
    }


    void
    AsciiFile::log_content() const
    {
	y2mil("content of " << name);

	for (const string& line : lines)
	    y2mil(line);
    }


    bool
    SysconfigFile::getValue(const string& key, string& value) const
    {
	Regex rx('^' + Regex::ws + key + '=' + "(['\"]?)([^'\"]*)\\1" + Regex::ws + '$');

	if (find_if(get_lines(), regex_matches(rx)) == get_lines().end())
	    return false;

	value = rx.cap(2);
	y2mil("key:" << key << " value:" << value);
	return true;
    }


    bool
    InstallInfFile::getValue(const string& key, string& value) const
    {
	Regex rx('^' + key + ":" + Regex::ws + "([^ ]*)" + '$');

	if (find_if(get_lines(), regex_matches(rx)) == get_lines().end())
	    return false;

	value = rx.cap(1);
	y2mil("key:" << key << " value:" << value);
	return true;
    }

}
