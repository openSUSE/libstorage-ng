/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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
#include <boost/algorithm/string.hpp>

#include "storage/Utils/FileUtils.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/Format.h"


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

	    ST_THROW(Exception(sformat("mkdtemp failed, %s", stringerror(errno))));
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
	    y2err("rmdir '" << get_fullname() << "' failed, " << stringerror(errno));
	}
    }


    string
    TmpDir::default_path()
    {
	const char* tmpdir = getenv("TMPDIR");
	return tmpdir ? tmpdir : "/tmp";
    }


    TmpMount::TmpMount(const string& path, const string& name_template, const string& device,
		       bool read_only, const vector<string>& options)
	: TmpDir(path, name_template)
    {
	SystemCmd::Args cmd_args = { MOUNT_BIN };

	// TODO also check options for 'ro' and 'rw'?

	if (read_only)
	    cmd_args << "--read-only";

	cmd_args << device << get_fullname();

	if (!options.empty())
	    cmd_args << "-o" << boost::join(options, ",");

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    TmpMount::~TmpMount()
    {
	try
	{
	    SystemCmd cmd({ UMOUNT_BIN, get_fullname() }, SystemCmd::DoThrow);
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    y2err("tmp unmount '" << get_fullname() << "' failed");
	}
    }

}
