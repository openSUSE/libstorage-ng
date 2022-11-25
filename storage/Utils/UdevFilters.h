/*
 * Copyright (c) 2022 SUSE LLC
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


#ifndef UDEV_FILTERS_H
#define UDEV_FILTERS_H


#include "storage/Utils/JsonFile.h"


namespace storage
{
    using namespace std;


    // See doc/udev.md.


    class UdevFilter
    {
    public:

	vector<string> allowed_path_patterns;
	vector<string> allowed_id_patterns;

	friend std::ostream& operator<<(std::ostream& s, const UdevFilter& udev_filter);

    };


    class UdevFilters
    {
    public:

	UdevFilters();

	UdevFilter disk;
	UdevFilter dasd;
	UdevFilter md;
	UdevFilter dm_raid;
	UdevFilter multipath;

	friend std::ostream& operator<<(std::ostream& s, const UdevFilters& udev_filters);

    private:

	void load(const string& filename);

	void parse1(json_object* root, const char* name, UdevFilter& udev_filter) const;
	UdevFilter parse2(json_object* object) const;
	vector<string> parse3(json_object* object) const;

    };


    /**
     * Remove names not matching any pattern. Matching names are ordered according to the
     * order of the patterns.
     */
    void
    udev_filter(vector<string>& names, const vector<string>& allowed_patterns);

}


#endif
