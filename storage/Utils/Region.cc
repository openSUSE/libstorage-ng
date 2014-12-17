/*
 * Copyright (c) [2004-2014] Novell, Inc.
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

    Region::Region()
	: impl(new Impl())
    {
    }


    Region::Region(unsigned long long start, unsigned long long len)
        : impl(new Impl(start, len))
    {
    }


    Region::~Region()
    {
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
