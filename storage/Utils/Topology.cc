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


#include "storage/Utils/TopologyImpl.h"


namespace storage
{


    Topology::Topology()
	: impl(make_unique<Impl>())
    {
    }


    Topology::Topology(long alignment_offset, unsigned long optimal_io_size)
	: impl(make_unique<Impl>(alignment_offset, optimal_io_size))
    {
    }


    Topology::Topology(const Topology& topology)
	: impl(make_unique<Impl>(topology.get_impl()))
    {
    }


    Topology::~Topology() = default;


    Topology&
    Topology::operator=(const Topology& topology)
    {
	*impl = topology.get_impl();
	return *this;
    }


    Topology::Impl&
    Topology::get_impl()
    {
        return *impl;
    }


    const Topology::Impl&
    Topology::get_impl() const
    {
        return *impl;
    }


    long
    Topology::get_alignment_offset() const
    {
        return get_impl().get_alignment_offset();
    }


    void
    Topology::set_alignment_offset(long alignment_offset)
    {
	get_impl().set_alignment_offset(alignment_offset);
    }


    unsigned long
    Topology::get_optimal_io_size() const
    {
        return get_impl().get_optimal_io_size();
    }


    void
    Topology::set_optimal_io_size(unsigned long optimal_io_size)
    {
	get_impl().set_optimal_io_size(optimal_io_size);
    }


    unsigned long
    Topology::get_minimal_grain() const
    {
	return get_impl().get_minimal_grain();
    }


    void
    Topology::set_minimal_grain(unsigned long minimal_grain)
    {
	get_impl().set_minimal_grain(minimal_grain);
    }


    bool
    Topology::operator==(const Topology& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Topology::operator!=(const Topology& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    std::ostream&
    operator<<(std::ostream& s, const Topology& topology)
    {
	return operator<<(s, topology.get_impl());
    }


    bool
    getChildValue(const xmlNode* node, const char* name, Topology& value)
    {
	return getChildValue(node, name, value.get_impl());
    }


    void
    setChildValue(xmlNode* node, const char* name, const Topology& value)
    {
	setChildValue(node, name, value.get_impl());
    }

}
