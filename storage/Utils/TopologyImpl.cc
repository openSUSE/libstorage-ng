/*
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


#include "storage/Utils/HumanString.h"
#include "storage/Utils/TopologyImpl.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    void
    Topology::Impl::set_alignment_offset(long alignment_offset)
    {
	Impl::alignment_offset = alignment_offset;
    }


    void
    Topology::Impl::set_optimal_io_size(unsigned long optimal_io_size)
    {
	Impl::optimal_io_size = optimal_io_size;
    }


    unsigned long
    Topology::Impl::calculate_grain() const
    {
	unsigned long grain = max(optimal_io_size, 1UL);

	while (grain < 1 * MiB)
	    grain *= 2;

	return grain;
    }


    bool
    Topology::Impl::align_block_in_place(unsigned long long& block, unsigned long block_size,
					 Location location) const
    {
	long alignment_offset_in_blocks = alignment_offset / block_size;
	unsigned long grain_in_blocks = calculate_grain() / block_size;

	block -= alignment_offset_in_blocks;

	switch (location)
	{
	    case Location::START: {
		unsigned long long rest = block % grain_in_blocks;
		if (rest != 0)
		    block += grain_in_blocks - rest;
	    } break;

	    case Location::END: {
		unsigned long long rest = (block + 1) % grain_in_blocks;
		if (rest > block)
		    return false;
		if (rest != 0)
		    block -= rest;
	    } break;
	}

	block += alignment_offset_in_blocks;

	return true;
    }


    bool
    Topology::Impl::align_in_place(Region& region, AlignPolicy align_policy) const
    {
	unsigned long block_size = region.get_block_size();

	unsigned long long start = region.get_start();
	if (!align_block_in_place(start, block_size, Location::START))
	    return false;

	unsigned long long length = 0;
	switch (align_policy)
	{
	    case AlignPolicy::ALIGN_END: {
		// TODO logical
		unsigned long long end = region.get_end();
		if (!align_block_in_place(end, block_size, Location::END))
		    return false;
		if (end < start)
		    return false;
		length = end - start + 1;
	    } break;

	    case AlignPolicy::KEEP_SIZE: {
		length = region.get_length();
	    } break;

	    case AlignPolicy::KEEP_END: {
		if (region.get_end() < start - region.get_start())
		    return false;
		length = region.get_length() - (start - region.get_start());
	    } break;
	}

	region = Region(start, length, region.get_block_size());
	return true;
    }


    bool
    Topology::Impl::can_be_aligned(const Region& region, AlignPolicy align_policy) const
    {
	Region tmp(region);
	return align_in_place(tmp, align_policy);
    }


    Region
    Topology::Impl::align(const Region& region, AlignPolicy align_policy) const
    {
	Region tmp(region);
	if (!align_in_place(tmp, align_policy))
	    ST_THROW(AlignError());

	return tmp;
    }


    bool
    Topology::Impl::operator==(const Topology::Impl& rhs) const
    {
	return alignment_offset == rhs.alignment_offset &&
	    optimal_io_size == rhs.optimal_io_size;
    }


    std::ostream&
    operator<<(std::ostream& s, const Topology::Impl& impl)
    {
	return s << "[" << impl.alignment_offset << " B, " << impl.optimal_io_size << " B]";
    }


    bool
    getChildValue(const xmlNode* node, const char* name, Topology::Impl& value)
    {
	const xmlNode* tmp = getChildNode(node, name);
	if (!tmp)
	    return false;

	getChildValue(tmp, "alignment-offset", value.alignment_offset);
	getChildValue(tmp, "optimal-io-size", value.optimal_io_size);

	return true;
    }


    void
    setChildValue(xmlNode* node, const char* name, const Topology::Impl& value)
    {
	xmlNode* tmp = xmlNewChild(node, name);

	setChildValueIf(tmp, "alignment-offset", value.alignment_offset, value.alignment_offset != 0);
	setChildValueIf(tmp, "optimal-io-size", value.optimal_io_size, value.optimal_io_size != 0);
    }

}
