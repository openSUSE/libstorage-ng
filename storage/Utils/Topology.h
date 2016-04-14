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

	/**
	 *
	 */
	unsigned long calculate_grain() const;

	/**
	 * alignable
	 */
	bool can_be_aligned(const Region& region, AlignPolicy align_policy) const;

	/**
	 *
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
