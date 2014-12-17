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


#ifndef REGION_H
#define REGION_H

#include <algorithm>

#include "storage/StorageInterface.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{
    using namespace storage_legacy;


    class Region
    {
    public:

	Region() : s(0), l(0) {}
	Region(unsigned long long start, unsigned long long len) : s(start), l(len) {}
	Region(const RegionInfo& region_info) : s(region_info.start), l(region_info.len) {}

	bool doIntersect( const Region& r ) const
	    { return( r.start() <= end() && r.end() >= start() ); }
	Region intersect( const Region& r ) const
	    {
	    if (doIntersect(r))
		{
		unsigned long long s = std::max(r.start(), start());
		unsigned long long e = std::min(r.end(), end());
		return Region(s, e - s + 1);
		}
	    return Region(0, 0);
	    }
	bool inside( const Region& r ) const
	    { return( start()>=r.start() && end() <= r.end() ); }
	bool operator==(const Region& r) const
	    { return( r.start()==s && r.len()==l ); }
	bool operator!=(const Region& r) const
	    { return( ! (*this==r) ); }
	bool operator<(const Region& r) const
	    { return( s < r.start() ); }
	bool operator>(const Region& r) const
	    { return( s > r.start() ); }

	unsigned long long start() const { return s; }
	unsigned long long len() const { return l; }
	unsigned long long end() const { return s + l - 1; }

	bool empty() const { return l == 0; }

	void setStart(unsigned long long start) { s = start; }
	void setLen(unsigned long long len) { l = len; }

	template <typename Type> friend
	Region operator*(Type i, const Region& r)
	{
	    static_assert(std::is_integral<Type>::value, "not integral");
	    return Region(i * r.s, i * r.l);
	}

	template <typename Type> friend
	Region operator/(const Region& r, Type i)
	{
	    static_assert(std::is_integral<Type>::value, "not integral");
	    return Region(r.s / i, r.l / i);
	}

	friend std::ostream& operator<<(std::ostream& s, const Region& p);

	friend bool getChildValue(const xmlNode* node, const char* name, Region& value);
	friend void setChildValue(xmlNode* node, const char* name, const Region& value);

	operator RegionInfo() const { return RegionInfo(s, l); }

    protected:

	unsigned long long s;
	unsigned long long l;

    };

}

#endif
