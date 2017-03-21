/*
 * Copyright (c) [2004-2014] Novell, Inc.
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

	DasdType get_dasd_type() const { return dasd_type; }
	DasdFormat get_dasd_format() const { return dasd_format; }

    private:

	void parse(const std::vector<string>& lines);

	string device;

	DasdType dasd_type;
	DasdFormat dasd_format;

    };

}

#endif
