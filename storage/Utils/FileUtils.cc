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


namespace storage
{

    TmpDir::TmpDir(const string& name_template)
	: path(), name(name_template)
    {
	const char* tmpdir = getenv("TMPDIR");
	path = tmpdir ? tmpdir : "/tmp";

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


    TmpDir::~TmpDir()
    {
	if (rmdir(get_fullname().c_str()) != 0)
	{
	    y2err("rmdir '" << get_fullname() << "' failed, " << strerror(errno));
	}
    }

}
