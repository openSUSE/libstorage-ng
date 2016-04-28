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


#ifndef STORAGE_TOPOLOGY_H
#define STORAGE_TOPOLOGY_H


#include <libxml/tree.h>
#include <memory>
#include <ostream>

#include "storage/Utils/Region.h"
#include "storage/Utils/Exception.h"


namespace storage
{

    enum class AlignPolicy
    {
	ALIGN_END, KEEP_END, KEEP_SIZE
    };


    class AlignError : public Exception
    {
    public:
	AlignError();
    };


    /**
     * A class to calculate partition alignment based on hardware
     * topology. The starting sector of a partition must obey the form "sector
     * = offset + X * grain". Alignment may fail is the start of the aligned
     * partition region is after the end of the aligned partition region.
     */
    class Topology
    {
    public:

	Topology();
	Topology(long alignment_offset, unsigned long optimal_io_size);
	Topology(const Topology& topology);
	~Topology();

	Topology& operator=(const Topology& topology);

	long get_alignment_offset() const;
	void set_alignment_offset(long alignment_offset);

	unsigned long get_optimal_io_size() const;
	void set_optimal_io_size(unsigned long optimal_io_size);

	unsigned long get_minimal_grain() const;
	void set_minimal_grain(unsigned long minimal_grain);

	/**
	 * Calculates the grain.
	 */
	unsigned long calculate_grain() const;

	/**
	 * Checks whether a region can be aligned. Alignment may fail if the
	 * aligned end is before the aligned start.
	 */
	bool can_be_aligned(const Region& region, AlignPolicy align_policy) const;

	/**
	 * Aligns a region.
	 * @throw AlignError
	 */
	Region align(const Region& region, AlignPolicy align_policy = AlignPolicy::ALIGN_END) const;

	bool operator==(const Topology& rhs) const;
	bool operator!=(const Topology& rhs) const;

	friend std::ostream& operator<<(std::ostream& s, const Topology& topology);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	friend bool getChildValue(const xmlNode* node, const char* name, Topology& topology);
	friend void setChildValue(xmlNode* node, const char* name, const Topology& topology);

    private:

	std::shared_ptr<Impl> impl;

    };

}


#endif
