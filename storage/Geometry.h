/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#ifndef STORAGE_GEOMETRY_H
#define STORAGE_GEOMETRY_H

#include "storage/Utils/XmlFile.h"


namespace storage
{

    struct Geometry
    {
	Geometry();
	Geometry(unsigned long cylinders, unsigned int heads, unsigned int sectors,
		 unsigned int sector_size)
	    : cylinders(cylinders), heads(heads), sectors(sectors), sector_size(sector_size)
	{}

	unsigned long cylinders;
	unsigned int heads;
	unsigned int sectors;
	unsigned int sector_size;

	unsigned long long sizeK() const;

	unsigned long cylinderSize() const;
	unsigned long headSize() const;

	unsigned long long cylinderToKb(unsigned long cylinder) const;
	unsigned long kbToCylinder(unsigned long long kb) const;

	unsigned long long sectorToKb(unsigned long long sector) const;
	unsigned long long kbToSector(unsigned long long kb) const;

	bool operator==(const Geometry& rhs) const;
	bool operator!=(const Geometry& rhs) const { return !(*this == rhs); }

	friend std::ostream& operator<<(std::ostream& s, const Geometry& geo);

	friend bool getChildValue(const xmlNode* node, const char* name, Geometry& value);
	friend void setChildValue(xmlNode* node, const char* name, const Geometry& value);
    };


    bool detectGeometry(const string& device, Geometry& geometry);

}


#endif
