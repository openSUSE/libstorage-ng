/*
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


#ifndef STORAGE_ALIGNMENT_IMPL_H
#define STORAGE_ALIGNMENT_IMPL_H


#include "storage/Utils/Alignment.h"


namespace storage
{

    /**
     * Impl of Alignment class.
     *
     * Note: This class has no functions to query the topology or extra-grain.
     * This is on purpose since the implemnentation might change if further
     * alignment contraints are required. In that case a better
     * implemnentation might be to only keep one offset and one grain (of
     * sector = offset + X * grain). See parted sources for the background.
     */
    class Alignment::Impl
    {
    public:

	Impl(const Topology& topology, AlignType align_type)
	    : align_type(align_type), topology(topology), extra_grain(0) {}

	void set_extra_grain(unsigned long extra_grain) { Impl::extra_grain = extra_grain; }

	long offset() const;

	unsigned long calculate_grain() const;

	bool can_be_aligned(const Region& region, AlignPolicy align_policy) const;

	Region align(const Region& region, AlignPolicy align_policy) const;

	enum class Location { START, END };

	bool align_block_in_place(unsigned long long& block, unsigned long block_size,
				  Location location) const;

	bool align_region_in_place(Region& region, AlignPolicy align_policy) const;

	friend std::ostream& operator<<(std::ostream& s, const Impl& impl);

    private:

	AlignType align_type;

	Topology topology;
	unsigned long extra_grain;

    };

}


#endif
