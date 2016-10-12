/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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


#include "storage/Utils/RegionImpl.h"


namespace storage
{

    InvalidBlockSize::InvalidBlockSize(unsigned int block_size)
	: Exception(sformat("invalid block size %d", block_size), ERROR)
    {
    }


    DifferentBlockSizes::DifferentBlockSizes(unsigned int seen, unsigned int expected)
	: Exception(sformat("different block sizes, seen: %d, exception: %d", seen, expected), ERROR)
    {
    }


    NoIntersection::NoIntersection()
	: Exception("no intersection", WARNING)
    {
    }


    NotInside::NotInside()
	: Exception("not inside", WARNING)
    {
    }


    Region::Region()
	: impl(new Impl())
    {
    }


    Region::Region(unsigned long long start, unsigned long long len, unsigned int block_size)
        : impl(new Impl(start, len, block_size))
    {
    }


    Region::Region(const Region& region)
	: impl(new Impl(region.get_impl()))
    {
    }


    Region::~Region()
    {
    }


    Region&
    Region::operator=(const Region& region)
    {
	*impl = region.get_impl();
	return *this;
    }


    Region::Impl&
    Region::get_impl()
    {
        return *impl;
    }


    const Region::Impl&
    Region::get_impl() const
    {
        return *impl;
    }


    bool
    Region::empty() const
    {
        return get_impl().empty();
    }


    unsigned long long
    Region::get_start() const
    {
        return get_impl().get_start();
    }


    unsigned long long
    Region::get_length() const
    {
        return get_impl().get_length();
    }


    unsigned long long
    Region::get_end() const
    {
        return get_impl().get_end();
    }


    void
    Region::set_start(unsigned long long start)
    {
	get_impl().set_start(start);
    }


    void
    Region::set_length(unsigned long long length)
    {
	get_impl().set_length(length);
    }


    void
    Region::adjust_start(long long delta)
    {
	get_impl().adjust_start(delta);
    }


    void
    Region::adjust_length(long long delta)
    {
	get_impl().adjust_length(delta);
    }


    unsigned int
    Region::get_block_size() const
    {
        return get_impl().get_block_size();
    }


    void
    Region::set_block_size(unsigned int block_size)
    {
        get_impl().set_block_size(block_size);
    }


    unsigned long long
    Region::to_bytes(unsigned long long blocks) const
    {
	return get_impl().to_bytes(blocks);
    }


    unsigned long long
    Region::to_blocks(unsigned long long bytes) const
    {
	return get_impl().to_blocks(bytes);
    }


    bool
    Region::operator==(const Region& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Region::operator!=(const Region& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    bool
    Region::operator<(const Region& rhs) const
    {
	return get_impl().operator<(rhs.get_impl());
    }


    bool
    Region::operator>(const Region& rhs) const
    {
	return get_impl().operator>(rhs.get_impl());
    }


    bool
    Region::inside(const Region& rhs) const
    {
	return get_impl().inside(rhs.get_impl());
    }


    bool
    Region::intersect(const Region& rhs) const
    {
	return get_impl().intersect(rhs.get_impl());
    }


    Region
    Region::intersection(const Region& rhs) const
    {
	return get_impl().intersection(rhs.get_impl());
    }


    vector<Region>
    Region::unused_regions(const vector<Region>& used_regions) const
    {
	return get_impl().unused_regions(used_regions);
    }


    std::ostream&
    operator<<(std::ostream& s, const Region& region)
    {
	return operator<<(s, region.get_impl());
    }


    bool
    getChildValue(const xmlNode* node, const char* name, Region& value)
    {
	return getChildValue(node, name, value.get_impl());
    }


    void
    setChildValue(xmlNode* node, const char* name, const Region& value)
    {
	setChildValue(node, name, value.get_impl());
    }

}
