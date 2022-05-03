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


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/LockImpl.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/StorageTmpl.h"


#define LOCK_DIR "/run/libstorage-ng"


namespace storage
{


    vector<const Lock*> Lock::locks;

    int Lock::fd = -1;


    Lock::Lock(bool read_only, bool disable)
	: read_only(read_only), disabled(disable)
    {
	if (disabled)
	    return;

	y2mil("getting " << (read_only ? "read-only" : "read-write") << " lock");

	if (locks.empty())
	{
	    // If there are no locks within the same process try to take the
	    // system-wide lock.

	    if (mkdir(LOCK_DIR, 0755) == -1 && errno != EEXIST)
	    {
		y2err("creating directory for lock-file failed: " << stringerror(errno));
	    }

	    fd = open(LOCK_DIR "/lock", (read_only ? O_RDONLY : O_WRONLY) | O_CREAT | O_CLOEXEC, 0600);
	    if (fd < 0)
	    {
		// Opening lock-file failed.
		y2err("opening lock-file failed: " << stringerror(errno));
		ST_THROW(LockException(0));
	    }

	    struct flock lock;
	    memset(&lock, 0, sizeof(lock));
	    lock.l_type = (read_only ? F_RDLCK : F_WRLCK);
	    lock.l_whence = SEEK_SET;
	    if (fcntl(fd, F_SETLK, &lock) < 0)
	    {
		switch (errno)
		{
		    case EACCES:
		    case EAGAIN:
			// Another process has a lock. Between the two fcntl
			// calls the lock of the other process could be
			// release. In that case we don't get the pid (and it is
			// still 0).
			fcntl(fd, F_GETLK, &lock);
			close(fd);
			y2err("locked by process " << lock.l_pid);
			ST_THROW(LockException(lock.l_pid));

		    default:
			// Some other error.
			close(fd);
			y2err("getting lock failed: " << stringerror(errno));
			ST_THROW(LockException(0));
		}
	    }
	}
	else
	{
	    // If there are locks within the same process check if a further
	    // lock is allowed.

	    if (read_only)
	    {
		// no read-write lock of the process allowed

		if (any_of(locks.begin(), locks.end(), [](const Lock* tmp) { return !tmp->read_only; }))
		{
		    y2err("read-write lock by same process exists");
		    ST_THROW(LockException(-2));
		}
	    }
	    else
	    {
		// no other lock of the process allowed

		y2err("lock by same process exists");
		ST_THROW(LockException(-2));
	    }
	}

	// Add this lock to the list of locks in the same process.

	locks.push_back(this);

	y2mil("lock succeeded");
    }


    Lock::~Lock() noexcept
    {
	if (disabled)
	    return;

	// Remove this lock from the list of locks in the same process.

	erase(locks, this);

	if (locks.empty())
	{
	    // If this process has no further locks release the system-wide
	    // lock.

	    close(fd);
	    fd = -1;

	    // Do not bother deleting lock-file. Likelihood of race conditions
	    // is to high.
	}
    }

}
