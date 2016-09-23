/*
 * Copyright (c) 2015 Novell, Inc.
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


#include <string.h>
#include <unistd.h>

#include "storage/Utils/FileUtils.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    TmpDir::TmpDir(const string& path, const string& name_template)
	: path(path), name(name_template)
    {
	char* t = strdup(get_fullname().c_str());
	if (!t)
	{
	    ST_THROW(Exception("strdup failed"));
	}

	if (mkdtemp(t) == NULL)
	{
	    free(t);

	    ST_THROW(Exception(sformat("mkdtemp failed, %s", strerror(errno))));
	}

	name = string(&t[strlen(t) - name.size()]);

	free(t);
    }


    TmpDir::TmpDir(const string& name_template)
	: TmpDir(default_path(), name_template)
    {
    }


    TmpDir::~TmpDir()
    {
	if (rmdir(get_fullname().c_str()) != 0)
	{
	    y2err("rmdir '" << get_fullname() << "' failed, " << strerror(errno));
	}
    }


    string
    TmpDir::default_path()
    {
	const char* tmpdir = getenv("TMPDIR");
	return tmpdir ? tmpdir : "/tmp";
    }


    TmpMount::TmpMount(const string& path, const string& name_template, const string& device)
	: TmpDir(path, name_template)
    {
	string cmd_line = MOUNTBIN " --read-only " + quote(device) + " " + quote(get_fullname());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("tmp mount failed"));
    }


    TmpMount::~TmpMount()
    {
	SystemCmd cmd(UMOUNTBIN " " + quote(get_fullname()));
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("tmp umount failed"));
    }

}
