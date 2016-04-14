/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) 2015 SUSE LLC
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
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    Region::Impl::Impl(unsigned long long start, unsigned long long length, unsigned int block_size)
	: start(start), length(length), block_size(block_size)
    {
	assert_valid_block_size();
    }


    std::ostream&
    operator<<(std::ostream& s, const Region::Impl& impl)
    {
	return s << "[" << impl.start << ", " << impl.length << ", " << impl.block_size << " B]";
    }


    void
    Region::Impl::assert_valid_block_size() const
    {
	if (block_size == 0)
	    ST_THROW(InvalidBlockSize(block_size));
    }


    void
    Region::Impl::assert_valid_block_size(unsigned int block_size) const
    {
	if (block_size == 0)
	    ST_THROW(InvalidBlockSize(block_size));
    }


    void
    Region::Impl::assert_equal_block_size(const Impl& rhs) const
    {
	if (block_size != rhs.block_size)
	    ST_THROW(DifferentBlockSizes(block_size, rhs.block_size));
    }


    void
    Region::Impl::set_block_size(unsigned int block_size)
    {
	assert_valid_block_size(block_size);

	Impl::block_size = block_size;
    }


    unsigned long long
    Region::Impl::to_bytes(unsigned long long blocks) const
    {
	assert_valid_block_size();

	return blocks * block_size;
    }


    unsigned long long
    Region::Impl::to_blocks(unsigned long long bytes) const
    {
	assert_valid_block_size();

	return bytes / block_size;
    }


    bool
    Region::Impl::operator==(const Impl& rhs) const
    {
	assert_equal_block_size(rhs);

	return start == rhs.get_start() && length == rhs.get_length();
    }


    bool
    Region::Impl::operator<(const Impl& rhs) const
    {
	assert_equal_block_size(rhs);

	return start < rhs.get_start();
    }


    bool
    Region::Impl::operator>(const Impl& rhs) const
    {
	assert_equal_block_size(rhs);

	return start > rhs.get_start();
    }


    bool
    Region::Impl::inside(const Impl& rhs) const
    {
	assert_equal_block_size(rhs);

	return get_start() >= rhs.get_start() && get_end() <= rhs.get_end();
    }


    bool
    Region::Impl::intersect(const Impl& rhs) const
    {
	assert_equal_block_size(rhs);

	return rhs.get_start() <= get_end() && rhs.get_end() >= get_start();
    }


    Region
    Region::Impl::intersection(const Impl& rhs) const
    {
	if (!intersect(rhs))
	    ST_THROW(NoIntersection());

	unsigned long long s = std::max(rhs.get_start(), get_start());
	unsigned long long e = std::min(rhs.get_end(), get_end());

	return Region(s, e - s + 1, block_size);
    }


    bool
    getChildValue(const xmlNode* node, const char* name, Region::Impl& value)
    {
	const xmlNode* tmp = getChildNode(node, name);
	if (!tmp)
	    return false;

	getChildValue(tmp, "start", value.start);
	getChildValue(tmp, "length", value.length);

	getChildValue(tmp, "block-size", value.block_size);

	return true;
    }


    void
    setChildValue(xmlNode* node, const char* name, const Region::Impl& value)
    {
	xmlNode* tmp = xmlNewChild(node, name);

	setChildValueIf(tmp, "start", value.start, value.start != 0);
	setChildValue(tmp, "length", value.length);

	setChildValue(tmp, "block-size", value.block_size);
    }

}
