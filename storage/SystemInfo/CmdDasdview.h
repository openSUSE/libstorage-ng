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


#include "storage/StorageInterface.h"
#include "storage/Geometry.h"
#include "storage/Devices/Disk.h"


namespace storage
{
    class SystemCmd;


    class Dasdview
    {

    public:

	Dasdview(const string& device);

	friend std::ostream& operator<<(std::ostream& s, const Dasdview& dasdview);

	const Geometry& getGeometry() const { return geometry; }
	DasdFormat getDasdFormat() const { return dasd_format; }
	DasdType getDasdType() const { return dasd_type; }

    private:

	void parse(const std::vector<string>& lines);

	string device;
	Geometry geometry;
	DasdFormat dasd_format;
	DasdType dasd_type;

    };

}

#endif
