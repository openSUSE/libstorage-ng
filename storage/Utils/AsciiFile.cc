/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2017-2018] SUSE LLC
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
#include <stdio.h>
#include <fcntl.h>
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>

#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{
    using namespace std;


    AsciiFile::AsciiFile(const string& name, bool remove_empty, int permissions)
	: name(name), remove_empty(remove_empty), permissions(permissions)
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


    void
    AsciiFile::save()
    {
	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	{
	    Mockup::set_file(name, lines);
	    return;
	}

	if (Mockup::get_mode() == Mockup::Mode::RECORD)
	{
	    Mockup::set_file(name, lines);
	}

	if (remove_empty && empty())
	{
	    y2mil("deleting file " << name);

	    if (unlink(name.c_str()) != 0 && errno != ENOENT)
		ST_THROW(IOException(sformat("Deleting file %s failed.", name.c_str())));
	}
	else
	{
	    y2mil("saving file " << name);

	    int fd = open(name.c_str(), O_WRONLY | O_TRUNC | O_CREAT | O_CLOEXEC, permissions);

	    if (fd < 0)
		ST_THROW(IOException(sformat("Opening file %s failed: %s", name.c_str(), strerror(errno))));

	    FILE* file = fdopen(fd, "we");

	    if (!file)
		ST_THROW(IOException(sformat("Opening file %s failed: %s", name.c_str(), strerror(errno))));

	    for (const string& line : lines)
	    {
		string line_with_break = line + "\n";

		fputs(line_with_break.c_str(), file);
	    }

	    if (ferror(file))
		ST_THROW(IOException(sformat("Saving file %s failed: %s", name.c_str(), strerror(errno))));

	    if (fclose(file) != 0)
		ST_THROW(IOException(sformat("Closing file %s failed: %s", name.c_str(), strerror(errno))));
	}
    }


    void
    AsciiFile::log_content() const
    {
	y2mil("content of " << name);

	for (const string& line : lines)
	    y2mil(line);
    }

}
