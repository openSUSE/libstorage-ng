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


#ifndef REGION_H
#define REGION_H


#include <libxml/tree.h>
#include <memory>

#include "storage/StorageInterface.h"


namespace storage
{
    using namespace storage_legacy;


    class Region
    {
    public:

	Region(unsigned long long start, unsigned long long length);
	Region(const RegionInfo& region) : Region(region.start, region.len) {}
	~Region();

	bool empty() const;

	unsigned long long get_start() const;
	unsigned long long get_length() const;
	unsigned long long get_end() const;

	void set_start(unsigned long long start);
	void set_length(unsigned long long length);

	bool operator==(const Region& rhs) const;
	bool operator!=(const Region& rhs) const;
	bool operator<(const Region& rhs) const;
	bool operator>(const Region& rhs) const;

	bool inside(const Region& rhs) const;

	bool intersect(const Region& rhs) const;
	Region intersection(const Region& rhs) const;

	friend std::ostream& operator<<(std::ostream& s, const Region& region);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	friend bool getChildValue(const xmlNode* node, const char* name, Region& value);
	friend void setChildValue(xmlNode* node, const char* name, const Region& value);

	operator RegionInfo() const { return RegionInfo(get_start(), get_length()); }

    private:

	std::shared_ptr<Impl> impl;

    };

}

#endif
