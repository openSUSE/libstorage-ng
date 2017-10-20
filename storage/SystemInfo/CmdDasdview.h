/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#ifndef STORAGE_CMD_DASDVIEW_H
#define STORAGE_CMD_DASDVIEW_H


#include "storage/Devices/Dasd.h"


namespace storage
{
    using std::string;

    class SystemCmd;


    class Dasdview
    {

    public:

	Dasdview(const string& device);

	friend std::ostream& operator<<(std::ostream& s, const Dasdview& dasdview);

	string get_bus_id() const { return bus_id; }

	DasdType get_type() const { return type; }
	DasdFormat get_format() const { return format; }

    private:

	void parse(const std::vector<string>& lines);

	string device;

	string bus_id;

	DasdType type;
	DasdFormat format;

    };

}

#endif
