/*
 * Copyright (c) [2004-2010] Novell, Inc.
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


#ifndef STORAGE_ASCII_FILE_H
#define STORAGE_ASCII_FILE_H


#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;


    class AsciiFile
    {
    public:

	explicit AsciiFile(const string& name, bool remove_empty = false, int permissions = DEFAULT_PERMISSIONS);

	const string& get_name() const { return name; }

	bool reload();

	/**
	 * @throw IOException
	 */
	void save();

	void log_content() const;

	bool empty() const { return lines.empty(); }

	void clear() { lines.clear(); }

	vector<string>& get_lines() { return lines; }
	const vector<string>& get_lines() const { return lines; }

	void set_lines(const vector<string>& lines) { this->lines = lines; }

    protected:

	// By default, file permissions are only limited by umask value
	static const int DEFAULT_PERMISSIONS = 0666;

	const string name;
	const bool remove_empty;
	const int permissions;

	vector<string> lines;

    };

}


#endif
