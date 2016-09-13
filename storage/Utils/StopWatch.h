/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#ifndef STORAGE_STOP_WATCH_H
#define STORAGE_STOP_WAtCH_H


#include <chrono>
#include <ostream>


namespace storage
{

    /**
     * Simple class implementing a stop watch.
     */
    class StopWatch
    {
    public:

	StopWatch();

	double read() const;

	friend std::ostream& operator<<(std::ostream& s, const StopWatch& sw);

    protected:

	std::chrono::steady_clock::time_point start_time;

    };

}

#endif
