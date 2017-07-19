/*
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


#ifndef STORAGE_LINES_ITERATOR_H
#define STORAGE_LINES_ITERATOR_H


#include <string>
#include <vector>


namespace storage
{

    using std::string;
    using std::vector;


    class LinesIterator
    {

    public:

	LinesIterator(const vector<string>& lines) : lines(lines), it(lines.begin()) {}

	bool at_end() const { return it == lines.end(); }

	const string& line();

	const string& pop_line();

    private:

	const vector<string>& lines;

	vector<string>::const_iterator it;

    };

}


#endif
