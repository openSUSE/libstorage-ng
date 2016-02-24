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


#ifndef STORAGE_REGION_H
#define STORAGE_REGION_H


#include <libxml/tree.h>
#include <memory>

#include "storage/Utils/Exception.h"


namespace storage
{

    class InvalidBlockSize : public Exception
    {
    public:
	InvalidBlockSize(unsigned int block_size);
    };


    class DifferentBlockSizes : public Exception
    {
    public:
	DifferentBlockSizes(unsigned int seen, unsigned int expected);
    };


    class NoIntersection : public Exception
    {
    public:
	NoIntersection();
    };


    /**
     * A start/length pair with a block size.
     *
     * Comparing Regions with different block_sizes will throw an exception.
     */
    class Region
    {
    public:

	Region();
	Region(unsigned long long start, unsigned long long length, unsigned int block_size);
	Region(const Region& region);
	~Region();

	Region& operator=(const Region& region);

	bool empty() const;

	unsigned long long get_start() const;
	unsigned long long get_length() const;
	unsigned long long get_end() const;

	void set_start(unsigned long long start);
	void set_length(unsigned long long length);

	unsigned int get_block_size() const;
	void set_block_size(unsigned int block_size);

	unsigned long long to_kb(unsigned long long value) const;
	unsigned long long to_value(unsigned long long kb) const;

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

    private:

	std::shared_ptr<Impl> impl;

    };

}

#endif
