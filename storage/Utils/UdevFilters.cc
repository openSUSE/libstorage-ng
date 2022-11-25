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


#include <fnmatch.h>

#include "storage/Utils/UdevFilters.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    std::ostream&
    operator<<(std::ostream& s, const UdevFilter& udev_filter)
    {
	s << "allowed-path-patterns:" << udev_filter.allowed_path_patterns
	  << " allowed-id-patterns:" << udev_filter.allowed_id_patterns;

	return s;
    }


    UdevFilters::UdevFilters()
    {
	// Only for check during build.
	const char* confdir = getenv("LIBSTORAGE_CONFDIR");
	if (confdir)
	{
	    load(string(confdir) + "/" UDEV_FILTERS_FILE);
	    return;
	}

	// While loading the filters from /etc only IO-Exceptions are caught. Thus if the
	// file exists it must also be valid.

	try
	{
	    y2mil("loading " << ETC_LIBSTORAGE_UDEV_FILTERS_FILE);
	    load(ETC_LIBSTORAGE_UDEV_FILTERS_FILE);
	}
	catch (const IOException& e)
	{
	    ST_CAUGHT(e);

	    y2mil("loading " << USR_SHARE_LIBSTORAGE_UDEV_FILTERS_FILE);
	    load(USR_SHARE_LIBSTORAGE_UDEV_FILTERS_FILE);
	}

	y2mil(*this);
    }


    void
    UdevFilters::load(const string& filename)
    {
	JsonFile json(filename);

	json_object* root = json.get_root();

	parse1(root, "Disk", disk);
	parse1(root, "Dasd", dasd);
	parse1(root, "Md", md);
	parse1(root, "DmRaid", dm_raid);
	parse1(root, "Multipath", multipath);
    }


    void
    UdevFilters::parse1(json_object* root, const char* name, UdevFilter& udev_filter) const
    {
	json_object* object2 = json_object_object_get(root, name);
	if (object2)
	    udev_filter = parse2(object2);
    }


    UdevFilter
    UdevFilters::parse2(json_object* object) const
    {
	UdevFilter ret;

	json_object* object2 = json_object_object_get(object, "allowed-path-patterns");
	if (object2)
	    ret.allowed_path_patterns = parse3(object2);

	json_object* object3 = json_object_object_get(object, "allowed-id-patterns");
	if (object3)
	    ret.allowed_id_patterns = parse3(object3);

	return ret;
    }


    vector<string>
    UdevFilters::parse3(json_object* object) const
    {
	vector<string> ret;

	if (!json_object_is_type(object, json_type_array))
	    ST_THROW(Exception("value of allowed-*-patterns entry not an array"));

	for (size_t i = 0; i < json_object_array_length(object); ++i)
	{
	    json_object* name = json_object_array_get_idx(object, i);
	    if (!json_object_is_type(name, json_type_string))
		ST_THROW(Exception("element of allowed-*-patterns array not a string"));

	    ret.push_back(json_object_get_string(name));
	}

	return ret;
    }


    std::ostream&
    operator<<(std::ostream& s, const UdevFilters& udev_filters)
    {
	s << "disk: " << udev_filters.disk << '\n';
	s << "dasd: " << udev_filters.dasd << '\n';
	s << "md: " << udev_filters.md << '\n';
	s << "dm-raid: " << udev_filters.dm_raid << '\n';
	s << "multipath: " << udev_filters.multipath << '\n';

	return s;
    }


    void
    udev_filter(vector<string>& names, const vector<string>& allowed_patterns)
    {
	const int flags = 0;

	vector<string>::iterator it = names.begin();

	for (const string& allowed_pattern : allowed_patterns)
	{
	    it = stable_partition(it, names.end(), [&allowed_pattern](const string& name) {
		return fnmatch(allowed_pattern.c_str(), name.c_str(), flags) == 0;
	    });
	}

	names.erase(it, names.end());
    }

}
