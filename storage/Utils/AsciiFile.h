/*
 * Copyright (c) [2004-2010] Novell, Inc.
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

	explicit AsciiFile(const string& name, bool remove_empty = false);

	const string& get_name() const { return name; }

	bool reload();
	bool save();

	void log_content() const;

	bool empty() const { return lines.empty(); }

	void clear() { lines.clear(); }

	vector<string>& get_lines() { return lines; }
	const vector<string>& get_lines() const { return lines; }

    protected:

	const string name;
	const bool remove_empty;

	vector<string> lines;

    };

}


#endif
