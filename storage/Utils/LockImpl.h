/*
 * Copyright (c) [2004-2009] Novell, Inc.
 * Copyright (c) 2018 SUSE LLC
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


#ifndef STORAGE_LOCK_IMPL_H
#define STORAGE_LOCK_IMPL_H


#include <vector>
#include <boost/noncopyable.hpp>

#include "storage/Utils/Lock.h"


namespace storage
{

    using namespace std;


    /**
     * Implement a system-wide read-only or read-write lock.
     *
     * Implemented using traditional ("process-associated") locks (for locks
     * across processes) and a global list (for locks within a single
     * process).
     *
     * An implementation using only open file descriptor (OFD) locks would be
     * simpler but does not provide the pid of the process holding a lock.
     */
    class Lock : private boost::noncopyable
    {

    public:

	Lock(bool read_only, bool disable = false);

	~Lock() noexcept;

    private:

	const bool read_only;
	const bool disabled;

	/**
	 * Locks within the same process.
	 */
	static vector<const Lock*> locks;

	/**
	 * File descriptor for system-wide lock.
	 */
	static int fd;

    };
}


#endif
