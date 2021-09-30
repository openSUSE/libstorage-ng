/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2018-2021] SUSE LLC
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
#include <optional>
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
    void operator>>(const string& d, std::optional<Value>& ov)
    {
	Value v;
	d >> v;
	ov = v;
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

	if (lhs == rhs)
	    return;

	log << " " << text << ":" << lhs << "-->" << rhs << "\n";
    }


    template <typename Type>
    void log_diff_hex(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(std::is_integral<Type>::value, "not integral");

	if (lhs == rhs)
	    return;

	boost::io::ios_all_saver ias(log);
	log << " " << text << ":" << std::showbase << std::hex << lhs << "-->" << rhs << "\n";
    }


    template <typename Type>
    void log_diff_enum(std::ostream& log, const char* text, const Type& lhs, const Type& rhs)
    {
	static_assert(std::is_enum<Type>::value, "not enum");

	if (lhs == rhs)
	    return;

	log << " " << text << ":" << toString(lhs) << "-->" << toString(rhs) << "\n";
    }


    template <typename Type>
    void log_diff(std::ostream& log, const char* text, const std::optional<Type>& lhs,
		  const std::optional<Type>& rhs)
    {
	if (lhs == rhs)
	    return;

	log << " " << text << ":";

	if (lhs == std::nullopt)
	    log << "none";
	else
	    log << lhs.value();

	log << "-->";

	if (rhs == std::nullopt)
	    log << "none";
	else
	    log << rhs.value();
    }


    inline
    void log_diff(std::ostream& log, const char* text, bool lhs, bool rhs)
    {
	if (lhs == rhs)
	    return;

	if (rhs)
	    log << " -->" << text << "\n";
	else
	    log << " " << text << "-->" << "\n";
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
    // http://en.cppreference.com/w/cpp/experimental/vector/erase
    template <class T, class U>
    void
    erase(vector<T>& c, const U& value)
    {
	c.erase(remove(c.begin(), c.end(), value), c.end());
    }


    // might become C++ standard, see
    // http://en.cppreference.com/w/cpp/experimental/vector/erase_if
    template <class T, class Predicate>
    void
    erase_if(vector<T>& c, Predicate pred)
    {
	c.erase(remove_if(c.begin(), c.end(), pred), c.end());
    }


    /**
     * Returns input sorted by the default comparison of the return value of the key
     * function fnc of each value in input. The ordering of elements with identical key is
     * stable.
     */
    template<typename ValueType, typename KeyType>
    vector<ValueType>
    sort_by_key(const vector<ValueType>& input, std::function<KeyType(ValueType)> fnc)
    {
	typedef typename vector<ValueType>::size_type size_type;

	std::multimap<KeyType, size_type> tmp;

	for (size_type i = 0; i < input.size(); ++i)
	    tmp.emplace(fnc(input[i]), i);

	vector<ValueType> output;

	for (const auto& t : tmp)
	    output.push_back(input[t.second]);

	return output;
    }

}

#endif
