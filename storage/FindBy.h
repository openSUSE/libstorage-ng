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


#ifndef STORAGE_FIND_BY_H
#define STORAGE_FIND_BY_H


namespace storage
{

    template<typename Type>
    Type*
    find_by_name(Devicegraph* devicegraph, const string& name)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    Type* device = dynamic_cast<Type*>(devicegraph->get_impl()[vertex]);
	    if (device && device->get_impl().get_name() == name)
		return device;
	}

	ST_THROW(DeviceNotFoundByName(name));
	__builtin_unreachable();
    }


    template<typename Type>
    const Type*
    find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const Type* device = dynamic_cast<const Type*>(devicegraph->get_impl()[vertex]);
	    if (device && device->get_impl().get_name() == name)
		return device;
	}

	ST_THROW(DeviceNotFoundByName(name));
	__builtin_unreachable();
    }


    template<typename Type>
    Type*
    find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    Type* device = dynamic_cast<Type*>(devicegraph->get_impl()[vertex]);
	    if (device && device->get_impl().get_uuid() == uuid)
		return device;
	}

	ST_THROW(DeviceNotFoundByUuid(uuid));
	__builtin_unreachable();
    }


    template<typename Type>
    const Type*
    find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const Type* device = dynamic_cast<const Type*>(devicegraph->get_impl()[vertex]);
	    if (device && device->get_impl().get_uuid() == uuid)
		return device;
	}

	ST_THROW(DeviceNotFoundByUuid(uuid));
	__builtin_unreachable();
    }

}


#endif
