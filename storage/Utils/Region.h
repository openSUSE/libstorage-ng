/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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
#include <vector>

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


    class NotInside : public Exception
    {
    public:
	NotInside();
    };


    /**
     * A start/length pair with a block size.
     *
     * The equation end = start + length - 1 holds.
     *
     * Comparing Regions with different block_sizes will throw an exception.
     */
    class Region
    {
    public:

	Region();
	Region(unsigned long long start, unsigned long long length, unsigned int block_size);
	Region(const Region& region);
	Region(Region&& region) = default;
	~Region();

	Region& operator=(const Region& region);
	Region& operator=(Region&& region) = default;

	bool empty() const;

	/**
	 * Returns the start of the region.
	 */
	unsigned long long get_start() const;

	/**
	 * Returns the length of the region.
	 */
	unsigned long long get_length() const;

	/**
	 * Returns the end of the region. Do not use this function if the
	 * region is empty since the end can wrap around in that case.
	 *
	 * @throw Exception
	 */
	unsigned long long get_end() const;

	/**
	 * Sets the start while keeping the length.
	 */
	void set_start(unsigned long long start);

	/**
	 * Sets the length while keeping the start.
	 */
	void set_length(unsigned long long length);

	/**
	 * Adjusts the start while keeping the length.
	 *
	 * @throw Exception
	 */
	void adjust_start(long long delta);

	/**
	 * Adjusts the length while keeping the start.
	 *
	 * @throw Exception
	 */
	void adjust_length(long long delta);

	unsigned int get_block_size() const;
	void set_block_size(unsigned int block_size);

	unsigned long long to_bytes(unsigned long long blocks) const;
	unsigned long long to_blocks(unsigned long long bytes) const;

	/**
	 * Compare start and length of two regions.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool operator==(const Region& rhs) const;

	/**
	 * Compare start and length of two regions.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool operator!=(const Region& rhs) const;

	/**
	 * Compare start of two regions.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool operator<(const Region& rhs) const;

	/**
	 * Compare start of two regions.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool operator>(const Region& rhs) const;

	/**
	 * Compare start of two regions.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool operator<=(const Region& rhs) const;

	/**
	 * Compare start of two regions.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool operator>=(const Region& rhs) const;

	/**
	 * Check whether the region is contained inside other.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool inside(const Region& rhs) const;

	/**
	 * Check whether the region intersects with other.
	 *
	 * @throw DifferentBlockSizes
	 */
	bool intersect(const Region& rhs) const;

	Region intersection(const Region& rhs) const;

	/**
	 * Returns all regions not included in used_regions. All block sizes
	 * must be identical.
	 *
	 * @throw Exception
	 */
	std::vector<Region> unused_regions(const std::vector<Region>& used_regions) const;

	friend std::ostream& operator<<(std::ostream& s, const Region& region);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	friend bool getChildValue(const xmlNode* node, const char* name, Region& value);
	friend void setChildValue(xmlNode* node, const char* name, const Region& value);

    private:

	const std::unique_ptr<Impl> impl;

    };

}

#endif
