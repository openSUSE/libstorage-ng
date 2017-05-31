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


namespace storage
{

    bool
    Topology::Impl::operator==(const Topology::Impl& rhs) const
    {
	return alignment_offset == rhs.alignment_offset &&
	    optimal_io_size == rhs.optimal_io_size && minimal_grain == rhs.minimal_grain;
    }


    std::ostream&
    operator<<(std::ostream& s, const Topology::Impl& impl)
    {
	s << "[" << impl.alignment_offset << " B, " << impl.optimal_io_size << " B";
	if (impl.minimal_grain != impl.default_minimal_grain)
	    s << ", " << impl.minimal_grain << " B";
	s << "]";

	return s;
    }


    bool
    getChildValue(const xmlNode* node, const char* name, Topology::Impl& value)
    {
	const xmlNode* tmp = getChildNode(node, name);
	if (!tmp)
	    return false;

	getChildValue(tmp, "alignment-offset", value.alignment_offset);
	getChildValue(tmp, "optimal-io-size", value.optimal_io_size);
	getChildValue(tmp, "minimal-grain", value.minimal_grain);

	return true;
    }


    void
    setChildValue(xmlNode* node, const char* name, const Topology::Impl& value)
    {
	xmlNode* tmp = xmlNewChild(node, name);

	setChildValueIf(tmp, "alignment-offset", value.alignment_offset, value.alignment_offset != 0);
	setChildValueIf(tmp, "optimal-io-size", value.optimal_io_size, value.optimal_io_size != 0);
	setChildValueIf(tmp, "minimal-grain", value.minimal_grain,
			value.minimal_grain != value.default_minimal_grain);
    }

}
