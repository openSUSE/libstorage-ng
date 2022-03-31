/*
 * Copyright (c) [2016-2018] SUSE LLC
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


#include <boost/integer/common_factor_rt.hpp>

#include "storage/Utils/AlignmentImpl.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    long
    Alignment::Impl::offset() const
    {
	return topology.get_alignment_offset();
    }


    unsigned long
    Alignment::Impl::calculate_grain() const
    {
	unsigned long grain = 1;

	if (align_type == AlignType::OPTIMAL)
	{
	    grain = std::max(topology.get_optimal_io_size(), grain);

	    while (grain < topology.get_minimal_grain())
		grain *= 2;
	}

	if (extra_grain > 0)
	    grain = boost::integer::lcm(grain, extra_grain);

	return grain;
    }


    bool
    Alignment::Impl::align_block_in_place(unsigned long long& block, unsigned long block_size,
					  Location location) const
    {
	long alignment_offset_in_blocks = offset() / block_size;

	unsigned long grain = std::max(calculate_grain(), block_size);
	unsigned long grain_in_blocks = grain / block_size;

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
		block -= rest;
	    } break;
	}

	block += alignment_offset_in_blocks;

	return true;
    }


    bool
    Alignment::Impl::align_region_in_place(Region& region, AlignPolicy align_policy) const
    {
	unsigned long block_size = region.get_block_size(ULL_HACK);

	unsigned long long start = region.get_start();

	if (align_policy != AlignPolicy::KEEP_START_ALIGN_END)
	{
	    if (!align_block_in_place(start, block_size, Location::START))
		return false;
	}

	unsigned long long length = 0;
	switch (align_policy)
	{
	    case AlignPolicy::KEEP_START_ALIGN_END:
	    case AlignPolicy::ALIGN_START_AND_END: {
		unsigned long long end = region.get_end();
		if (!align_block_in_place(end, block_size, Location::END))
		    return false;
		if (end + 1 <= start)
		    return false;
		length = end - start + 1;
	    } break;

	    case AlignPolicy::ALIGN_START_KEEP_SIZE: {
		length = region.get_length();
	    } break;

	    case AlignPolicy::ALIGN_START_KEEP_END: {
		unsigned long long delta = start - region.get_start();
		if (region.get_length() <= delta)
		    return false;
		length = region.get_length() - delta;
	    } break;
	}

	region = Region(start, length, region.get_block_size(ULL_HACK), ULL_HACK);
	return true;
    }


    bool
    Alignment::Impl::can_be_aligned(const Region& region, AlignPolicy align_policy) const
    {
	Region tmp(region);
	return align_region_in_place(tmp, align_policy);
    }


    Region
    Alignment::Impl::align(const Region& region, AlignPolicy align_policy) const
    {
	Region tmp(region);
	if (!align_region_in_place(tmp, align_policy))
	    ST_THROW(AlignError());

	return tmp;
    }


    std::ostream&
    operator<<(std::ostream& s, const Alignment::Impl& impl)
    {
	s << "topology:" << impl.topology;

	if (impl.extra_grain != 0)
	    s << " extra-grain:" << impl.extra_grain << " B";

	return s;
    }

}
