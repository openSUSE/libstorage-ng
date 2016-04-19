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


#ifndef STORAGE_TOPOLOGY_IMPL_H
#define STORAGE_TOPOLOGY_IMPL_H


#include "storage/Utils/Region.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Topology.h"


namespace storage
{

    class Topology::Impl
    {
    public:

	Impl() : alignment_offset(0), optimal_io_size(0) {}
	Impl(long alignment_offset, unsigned long optimal_io_size) :
	    alignment_offset(alignment_offset), optimal_io_size(optimal_io_size) {}

	long get_alignment_offset() const { return alignment_offset; }
	void set_alignment_offset(long alignment_offset);

	unsigned long get_optimal_io_size() const { return optimal_io_size; }
	void set_optimal_io_size(unsigned long optimal_io_size);

	unsigned long calculate_grain() const;

	bool can_be_aligned(const Region& region, AlignPolicy align_policy) const;

	Region align(const Region& region, AlignPolicy align_policy) const;

	bool operator==(const Impl& rhs) const;
	bool operator!=(const Impl& rhs) const { return !(*this == rhs); }

	friend std::ostream& operator<<(std::ostream& s, const Impl& impl);

	friend bool getChildValue(const xmlNode* node, const char* name, Impl& value);
	friend void setChildValue(xmlNode* node, const char* name, const Impl& value);

    private:

	enum class Location { START, END };

	bool align_block_in_place(unsigned long long& block, unsigned long block_size,
				  Location location) const;

	bool align_in_place(Region& region, AlignPolicy align_policy) const;

	long alignment_offset;
	unsigned long optimal_io_size;

    };

}


#endif
