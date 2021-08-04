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


#ifndef STORAGE_POOL_IMPL_H
#define STORAGE_POOL_IMPL_H


#include "storage/Pool.h"
#include "storage/Devices/Device.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;

    class Partitionable;


    class Pool::Impl : private boost::noncopyable
    {

    public:

	const map<string, string>& get_userdata() const { return userdata; }
	void set_userdata(const map<string, string>& userdata) { Impl::userdata = userdata; }

	void add_device(const Device* device);
	void remove_device(const Device* device);

	size_t size(const Devicegraph* devicegraph) const;

	vector<Device*> get_devices(Devicegraph* devicegraph) const;
	vector<const Device*> get_devices(const Devicegraph* devicegraph) const;

	/**
	 * Get the best partitionable candidates from the pool according to the strategy.
	 * The best candidates are at the front of the returned vector.
	 *
	 * So far only partitionables with a partition table are included.
	 *
	 * The result is notdeterministic.
	 */
	vector<Partitionable*> get_partitionable_candidates(Devicegraph* devicegraph) const;

	unsigned long long max_partition_size(Devicegraph* devicegraph, unsigned int number) const;

	vector<Partition*> create_partitions(Devicegraph* devicegraph, unsigned int number,
					     unsigned long long size) const;

    private:

	map<string, string> userdata;

	vector<sid_t> devices;

    };

}

#endif
