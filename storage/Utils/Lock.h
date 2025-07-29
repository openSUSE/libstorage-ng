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


#ifndef STORAGE_LOCK_H
#define STORAGE_LOCK_H


#include <sys/types.h>

#include "storage/Utils/Exception.h"


namespace storage
{

    /**
     * Exception indicating that getting the lock failed.
     */
    class LockException : public Exception
    {

    public:

	LockException(pid_t locker_pid);

	/**
	 * pid of one of the process holding a lock. The pid is 0 if it could
	 * not be determined, -1 if the lock is held by a open file descriptor
	 * lock and -2 if the lock is held by the same process. Note that the
	 * pid may already be out of date by the time the function returns.
	 */
	pid_t get_locker_pid() const;

    private:

	const pid_t locker_pid;

    };

}

#endif
