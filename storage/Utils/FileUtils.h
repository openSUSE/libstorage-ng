/*
 * Copyright (c) 2015 Novell, Inc.
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


#ifndef STORAGE_FILE_UTILS_H
#define STORAGE_FILE_UTILS_H


#include <string>


namespace storage
{

    using std::string;


    class TmpDir
    {

    public:

	/**
	 * Creates a temporary directory. For details see mkdtemp.
	 */
	TmpDir(const string& name_template);

	/**
	 * Deletes the temporary directory. Does not try to delete files
	 * inside the temporary directory.
	 */
	~TmpDir();

	const string& get_path() const { return path; }
	const string& get_name() const { return name; }

	string get_fullname() const { return path + "/" + name; }

    protected:

	string path;
	string name;

    };

}


#endif
