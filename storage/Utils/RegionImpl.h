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


#ifndef STORAGE_REGION_IMPL_H
#define STORAGE_REGION_IMPL_H

#include <algorithm>

#include "storage/Utils/Region.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{
    using namespace storage_legacy;


    class Region::Impl
    {
    public:

	Impl() : start(0), length(0) {}
	Impl(unsigned long long start, unsigned long long length) : start(start), length(length) {}

	bool empty() const { return length == 0; }

	unsigned long long get_start() const { return start; }
	unsigned long long get_length() const { return length; }
	unsigned long long get_end() const { return start + length - 1; }

	void set_start(unsigned long long start) { Impl::start = start; }
	void set_length(unsigned long long length) { Impl::length = length; }

	bool operator==(const Impl& rhs) const
	    { return start == rhs.get_start() && length == rhs.get_length(); }
	bool operator!=(const Impl& rhs) const
	    { return !(*this == rhs); }
	bool operator<(const Impl& rhs) const
	    { return start < rhs.get_start(); }
	bool operator>(const Impl& rhs) const
	    { return start > rhs.get_start(); }

	bool inside(const Impl& rhs) const
	    { return get_start() >= rhs.get_start() && get_end() <= rhs.get_end(); }

	bool intersect(const Impl& rhs) const
	    { return rhs.get_start() <= get_end() && rhs.get_end() >= get_start(); }

	Region intersection(const Impl& rhs) const;

	friend std::ostream& operator<<(std::ostream& s, const Impl& impl);

	friend bool getChildValue(const xmlNode* node, const char* name, Impl& value);
	friend void setChildValue(xmlNode* node, const char* name, const Impl& value);

    protected:

	unsigned long long start;
	unsigned long long length;

    };

}

#endif
