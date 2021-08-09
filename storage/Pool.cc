/*
 * Copyright (c) 2020 Arvin Schnell
 * Copyright (c) 2021 SUSE LLC
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


#include "storage/PoolImpl.h"


namespace storage
{

    using namespace std;


    PoolOutOfSpace::PoolOutOfSpace()
	: Exception("pool out of space")
    {
    }


    Pool::Pool()
	: impl(make_unique<Impl>())
    {
    }


    Pool::~Pool()
    {
    }


    const map<string, string>&
    Pool::get_userdata() const
    {
	return get_impl().get_userdata();
    }


    void
    Pool::set_userdata(const map<string, string>& userdata)
    {
	get_impl().set_userdata(userdata);
    }


    size_t
    Pool::size(const Devicegraph* devicegraph) const
    {
	return get_impl().size(devicegraph);
    }


    bool
    Pool::exists_device(const Device* device) const
    {
	return get_impl().exists_device(device);
    }


    void
    Pool::add_device(const Device* device)
    {
	get_impl().add_device(device);
    }


    void
    Pool::remove_device(const Device* device)
    {
	get_impl().remove_device(device);
    }


    vector<const Device*>
    Pool::get_devices(const Devicegraph* devicegraph) const
    {
	return get_impl().get_devices(devicegraph);
    }


    unsigned long long
    Pool::max_partition_size(Devicegraph* devicegraph, unsigned int number) const
    {
	return get_impl().max_partition_size(devicegraph, number);
    }


    vector<Partition*>
    Pool::create_partitions(Devicegraph* devicegraph, unsigned int number, unsigned long long size) const
    {
	return get_impl().create_partitions(devicegraph, number, size);
    }

}
