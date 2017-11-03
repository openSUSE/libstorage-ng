/*
 * Copyright (c) [2004-2009] Novell, Inc.
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
#include "storage/Utils/Lock.h"


#define LOCKDIR "/run/libstorage"


namespace storage
{

    LockException::LockException(pid_t locker_pid)
	: locker_pid(locker_pid)
    {
    }


    LockException::~LockException() noexcept
    {
    }


    Lock::Lock(bool read_only, bool disable)
	: disabled(disable || getenv("LIBSTORAGE_NO_LOCKING") != NULL),
	  fd(-1)
    {
	if (disabled)
	    return;

	y2mil("getting " << (read_only ? "read-only" : "read-write") << " lock");

	if (mkdir(LOCKDIR, 0755) == -1 && errno != EEXIST)
	{
	    y2err("creating directory for lock-file failed: " << strerror(errno));
	}

	fd = open(LOCKDIR "/lock", (read_only ? O_RDONLY : O_WRONLY) | O_CREAT | O_CLOEXEC,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fd < 0)
	{
	    // Opening lock-file failed.
	    y2err("opening lock-file failed: " << strerror(errno));
	    throw LockException(0);
	}

	struct flock lock;
	memset(&lock, 0, sizeof(lock));
	lock.l_whence = SEEK_SET;
	lock.l_type = (read_only ? F_RDLCK : F_WRLCK);
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
		    throw LockException(lock.l_pid);

		default:
		    // Some other error.
		    close(fd);
		    y2err("getting lock failed: " << strerror(errno));
		    throw LockException(0);
	    }
	}

	y2mil("lock succeeded");
    }


    Lock::~Lock() noexcept
    {
	if (disabled)
	    return;

	y2mil("releasing lock");
	close(fd);

	// Do not bother deleting lock-file. Likelihood of race conditions is
	// to high.
    }

}
