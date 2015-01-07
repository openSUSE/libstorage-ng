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


#ifndef CMD_PARTED_H
#define CMD_PARTED_H

#include "storage/StorageInterface.h"
#include "storage/Geometry.h"
#include "storage/Utils/Region.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    class Parted
    {

    public:

	Parted(const string& device);

	struct Entry
	{
	    Entry() : num(0), type(PRIMARY), id(0), boot(false) {}

	    unsigned num;
	    Region cylRegion;
	    Region secRegion;
	    PartitionType type;
	    unsigned id;
	    bool boot;
	};

	friend std::ostream& operator<<(std::ostream& s, const Parted& parted);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	PtType getLabel() const { return label; }
	bool getImplicit() const { return implicit; }
	const Geometry& getGeometry() const { return geometry; }
	bool getGptEnlarge() const { return gpt_enlarge; }

	typedef vector<Entry>::const_iterator const_iterator;

	const vector<Entry>& getEntries() const { return entries; }

	bool getEntry(unsigned num, Entry& entry) const;

    private:

	void parse(const vector<string>& lines);

	typedef vector<Entry>::iterator iterator;

	string device;
	PtType label;
	bool implicit;
	Geometry geometry;
	bool gpt_enlarge;
	vector<Entry> entries;

	void scanDiskFlags(const string& line);
	void scanSectorSizeLine(const string& line);
	void scanGeometryLine(const string& line);
	void scanCylEntryLine(const string& line);
	void scanSecEntryLine(const string& line);

    };

}


#endif
