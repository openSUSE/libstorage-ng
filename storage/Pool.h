/*
 * Copyright (c) 2020 Arvin Schnell
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


#ifndef STORAGE_POOL_H
#define STORAGE_POOL_H


#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>

#include "storage/Utils/Exception.h"


namespace storage
{

    class Devicegraph;
    class Device;
    class Partition;


    /**
     * Exception to report that the pool is out of space to fulfill the request.
     */
    class PoolOutOfSpace : public Exception
    {
    public:

	PoolOutOfSpace();

    };


    /**
     * A pool represents a collection of devices. This can be used to e.g. create the
     * partitions used of an MD.
     *
     * When requesting space from the pool the space is provided according to some strategy.
     *
     * There are many aspects to consider for a strategy:
     *
     * 1. Distribute bandwidth among all devices of the pool.
     *
     * 2. When generating several partition pairs (e.g. for RAIDs) all of the space of the
     * pool should be made available. This would fail if e.g. there are three disks in the
     * pool and several RAIDs are created on the first two disks of the pool. When the
     * first two disks are full no further RAIDs can be created leaving much of the pool
     * used.
     *
     * 3. When creating several small partitions there should still be enough space to
     * later create big partitions.
     *
     * The current strategy fulfills 1. and 2. by preferring devices which are so far less
     * used.
     *
     * All functions concerning pools are experimental.
     */
    class Pool : private boost::noncopyable
    {
    public:

	Pool();
	~Pool();

	/**
	 * Add a device to the pool. The devicegraph the device belongs to is irrelevant.
	 *
	 * @see remove_device(const Device*)
	 *
	 * @throw Exception
	 */
	void add_device(const Device* device);

	/**
	 * Remove a device from the pool. The devicegraph the device belongs to is irrelevant.
	 *
	 * @see add_device(const Device*)
	 *
	 * @throw Exception
	 */
	void remove_device(const Device* device);

	/**
	 * Get the devices of the pool available in the devicegraph. It is no error if
	 * devices are not available in the devicegraph.
	 */
	std::vector<const Device*> get_devices(const Devicegraph* devicegraph) const;

	/**
	 * Create a number of partitions of size in the pool. Devices in the pool not of
	 * type partitionable or without a partition table are ignored.
	 *
	 * The sizes of the created partitions may be different from size due to
	 * alignment. They may even be pairwise different.
	 *
	 * The result is nondeterministic.
	 *
	 * @throw Exception, PoolOutOfSpace
	 */
	std::vector<Partition*> create_partitions(Devicegraph* devicegraph, unsigned int number,
						  unsigned long long size) const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}


#endif
