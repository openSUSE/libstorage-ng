/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2015-2022] SUSE LLC
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

    class Region::Impl
    {
    public:

	Impl() : start(0), length(0), block_size(0) {}
	Impl(unsigned long long start, unsigned long long length, unsigned long long block_size);

	bool empty() const { return length == 0; }

	unsigned long long get_start() const { return start; }
	unsigned long long get_length() const { return length; }
	unsigned long long get_end() const;

	void set_start(unsigned long long start) { Impl::start = start; }
	void set_length(unsigned long long length) { Impl::length = length; }

	void adjust_start(long long delta);
	void adjust_length(long long delta);

	unsigned long long get_block_size() const { return block_size; }
	void set_block_size(unsigned long long block_size);

	unsigned long long to_bytes(unsigned long long blocks) const;
	unsigned long long to_blocks(unsigned long long bytes) const;

	bool operator==(const Impl& rhs) const;
	bool operator!=(const Impl& rhs) const { return !(*this == rhs); }

	bool operator<(const Impl& rhs) const;
	bool operator>(const Impl& rhs) const { return rhs < *this; }
	bool operator<=(const Impl& rhs) const { return !(*this > rhs); }
	bool operator>=(const Impl& rhs) const { return !(*this < rhs); }

	bool inside(const Impl& rhs) const;

	bool intersect(const Impl& rhs) const;
	Region intersection(const Impl& rhs) const;

	vector<Region> unused_regions(const vector<Region>& used_regions) const;

	friend std::ostream& operator<<(std::ostream& s, const Impl& impl);

	friend bool getChildValue(const xmlNode* node, const char* name, Impl& value);
	friend void setChildValue(xmlNode* node, const char* name, const Impl& value);

    protected:

	void assert_valid_block_size() const;
	void assert_valid_block_size(unsigned long long block_size) const;
	void assert_equal_block_size(const Impl& rhs) const;

	unsigned long long start = 0;
	unsigned long long length = 0;

	unsigned long long block_size = 0;

    };

}

#endif
