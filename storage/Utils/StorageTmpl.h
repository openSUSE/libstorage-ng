/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#ifndef STORAGE_STORAGE_TMPL_H
#define STORAGE_STORAGE_TMPL_H


#include <functional>
#include <algorithm>
#include <ostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <boost/io/ios_state.hpp>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    template<class Value>
    void operator>>(const string& d, Value& v)
    {
	std::istringstream Data(d);
	classic(Data);
	Data >> v;
    }


    template<class Value>
    std::ostream& operator<<(std::ostream& s, const std::list<Value>& l)
    {
	s << "<";
	for (typename std::list<Value>::const_iterator it = l.begin(); it != l.end(); ++it)
	{
	    if (it != l.begin())
		s << " ";
	    s << *it;
	}
	s << ">";
	return s;
    }


    template<class Value>
    std::ostream& operator<<(std::ostream& s, const std::vector<Value>& l)
    {
	s << "<";
	for (typename std::vector<Value>::const_iterator it = l.begin(); it != l.end(); ++it)
	{
	    if (it != l.begin())
		s << " ";
	    s << *it;
	}
	s << ">";
	return s;
    }


    template<class Value>
    std::ostream& operator<<(std::ostream& s, const std::set<Value>& l)
    {
	s << "<";
	for (typename std::set<Value>::const_iterator it = l.begin(); it != l.end(); ++it)
	{
	    if (it != l.begin())
		s << " ";
	    s << *it;
	}
	s << ">";
	return s;
    }


    template<class F, class S>
    std::ostream& operator<<(std::ostream& s, const std::pair<F, S>& p)
    {
	s << "[" << p.first << ":" << p.second << "]";
	return s;
    }


    template<class Key, class Value>
    std::ostream& operator<<(std::ostream& s, const std::map<Key, Value>& m)
    {
	s << "<";
	for (typename std::map<Key,Value>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
	    if (it != m.begin())
		s << " ";
	    s << it->first << ":" << it->second;
	}
	s << ">";
	return s;
    }


    template <typename Type>
    void log_diff(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(!std::is_enum<Type>::value, "is enum");

	if (lhs != rhs)
	    log << " " << text << ":" << lhs << "-->" << rhs;
    }


    template <typename Type>
    void log_diff_hex(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(std::is_integral<Type>::value, "not integral");

	if (lhs != rhs)
	{
	    boost::io::ios_all_saver ias(log);
	    log << " " << text << ":" << std::showbase << std::hex << lhs << "-->" << rhs;
	}
    }


    template <typename Type>
    void log_diff_enum(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(std::is_enum<Type>::value, "not enum");

	if (lhs != rhs)
	    log << " " << text << ":" << toString(lhs) << "-->" << toString(rhs);
    }


    inline
    void log_diff(std::ostream& log, const char* text, bool lhs, bool rhs)
    {
	if (lhs != rhs)
	{
	    if (rhs)
		log << " -->" << text;
	    else
		log << " " << text << "-->";
	}
    }


    template <typename ListType, typename Type>
    bool contains(const ListType& l, const Type& value)
    {
	return find(l.begin(), l.end(), value) != l.end();
    }


    template <typename ListType, typename Predicate>
    bool contains_if(const ListType& l, Predicate pred)
    {
	return find_if(l.begin(), l.end(), pred) != l.end();
    }


    // might become C++ standard, see
    // http://en.cppreference.com/w/cpp/experimental/vector/erase_if
    template <typename Type, typename Predicate>
    void
    erase_if(vector<Type>& v, Predicate pred)
    {
	v.erase(remove_if(v.begin(), v.end(), pred), v.end());
    }


    template <class T, unsigned int sz>
    inline unsigned int lengthof(T (&)[sz]) { return sz; }

}

#endif
