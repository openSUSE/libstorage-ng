/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_HOLDER_IMPL_H
#define STORAGE_HOLDER_IMPL_H


#include <libxml/tree.h>
#include <type_traits>

#include "storage/Utils/ExceptionImpl.h"
#include "storage/Holders/Holder.h"
#include "storage/DevicegraphImpl.h"


namespace storage
{

    using namespace std;


    template <typename Type> struct HolderTraits {};


    // abstract class

    class Holder::Impl
    {
    public:

	virtual ~Impl() {}

	bool operator==(const Impl& rhs) const;
	bool operator!=(const Impl& rhs) const { return !(*this == rhs); }

	Holder* copy_to_devicegraph(Devicegraph* dest) const;

	bool exists_in_devicegraph(const Devicegraph* devicegraph) const;
	bool exists_in_probed() const;
	bool exists_in_staging() const;
	bool exists_in_system() const;

	Storage* get_storage();
	const Storage* get_storage() const;

	virtual Impl* clone() const = 0;

	virtual const char* get_classname() const = 0;

	virtual bool is_in_view(View view) const { return true; }

	virtual void save(xmlNode* node) const = 0;

	void set_devicegraph_and_edge(Devicegraph* devicegraph,
				      Devicegraph::Impl::edge_descriptor edge);

	void set_edge(Devicegraph::Impl::edge_descriptor edge);

	Devicegraph* get_devicegraph() { return devicegraph; }
	const Devicegraph* get_devicegraph() const { return devicegraph; }

	Devicegraph::Impl::edge_descriptor get_edge() const { return edge; }

	virtual Holder* get_non_impl() { return devicegraph->get_impl()[edge]; }
	virtual const Holder* get_non_impl() const { return devicegraph->get_impl()[edge]; }

	Device* get_source();
	const Device* get_source() const;

	sid_t get_source_sid() const;

	Device* get_target();
	const Device* get_target() const;

	sid_t get_target_sid() const;

	const map<string, string>& get_userdata() const { return userdata; }
	void set_userdata(const map<string, string>& userdata) { Impl::userdata = userdata; }

	virtual bool equal(const Impl& rhs) const = 0;
	virtual void log_diff(std::ostream& log, const Impl& rhs) const = 0;
	virtual void print(std::ostream& out) const = 0;

    protected:

	Impl();

	Impl(const xmlNode* node);

    private:

	Devicegraph* devicegraph = nullptr;
	Devicegraph::Impl::edge_descriptor edge;

	map<string, string> userdata;

    };


    static_assert(std::is_abstract<Holder>(), "Holder ought to be abstract.");
    static_assert(std::is_abstract<Holder::Impl>(), "Holder::Impl ought to be abstract.");


    template <typename Type>
    bool is_holder_of_type(const Holder* holder)
    {
	static_assert(std::is_const<Type>::value, "Type must be const");

	ST_CHECK_PTR(holder);

	return dynamic_cast<const Type*>(holder);
    }


    template <typename Type>
    Type* to_holder_of_type(Holder* holder)
    {
	static_assert(!std::is_const<Type>::value, "Type must not be const");

	ST_CHECK_PTR(holder);

	Type* tmp = dynamic_cast<Type*>(holder);
	if (!tmp)
	    ST_THROW(HolderHasWrongType(holder->get_impl().get_classname(),
					HolderTraits<Type>::classname));

	return tmp;
    }


    template <typename Type>
    const Type* to_holder_of_type(const Holder* holder)
    {
	static_assert(std::is_const<Type>::value, "Type must be const");

	ST_CHECK_PTR(holder);

	const Type* tmp = dynamic_cast<const Type*>(holder);
	if (!tmp)
	    ST_THROW(HolderHasWrongType(holder->get_impl().get_classname(),
					HolderTraits<typename std::remove_const<Type>::type>::classname));

	return tmp;
    }

}

#endif
