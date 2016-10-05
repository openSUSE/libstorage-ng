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


#include <boost/io/ios_state.hpp>

#include "storage/Utils/Stopwatch.h"


namespace storage
{
    using namespace std;


    Stopwatch::Stopwatch()
	: start_time(chrono::steady_clock::now())
    {
    }


    double
    Stopwatch::read() const
    {
	chrono::steady_clock::time_point stop_time = chrono::steady_clock::now();
	chrono::steady_clock::duration duration = stop_time - start_time;
	return chrono::duration<double>(duration).count();
    }


    std::ostream&
    operator<<(std::ostream& s, const Stopwatch& stopwatch)
    {
	boost::io::ios_all_saver ias(s);
	return s << fixed << stopwatch.read() << "s";
    }

}
