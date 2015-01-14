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


#ifndef CMD_UDEVADM_H
#define CMD_UDEVADM_H


#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;


    class CmdUdevadmInfo
    {

    public:

	CmdUdevadmInfo(const string& file);

	const string& get_path() const { return path; }
	const string& get_name() const { return name; }

	dev_t get_majorminor() const { return majorminor; }
	unsigned int get_major() const { return gnu_dev_major(majorminor); }
	unsigned int get_minor() const { return gnu_dev_minor(majorminor); }

	const vector<string>& get_by_path_links() const { return by_path_links; }
	const vector<string>& get_by_id_links() const { return by_id_links; }

	friend std::ostream& operator<<(std::ostream& s, const CmdUdevadmInfo& cmdudevadminfo);

    private:

	void parse(const vector<string>& stdout);

	string file;

	string path;
	string name;

	dev_t majorminor;

	vector<string> by_path_links;
	vector<string> by_id_links;

    };

}


#endif
