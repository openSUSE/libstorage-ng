/*
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


#ifndef STORAGE_SYSTEM_INFO_H
#define STORAGE_SYSTEM_INFO_H


#include <memory>
#include <boost/noncopyable.hpp>


namespace storage
{

    /**
     * The SystemInfo class keeps various system information.
     *
     * So far the class can only be used for the BlkDevice::find_by_any_name() functions.
     * There it can avoid querying the same information from the system several times when
     * using several calls to BlkDevice::find_by_any_name().
     */
    class SystemInfo : private boost::noncopyable
    {
    public:

	SystemInfo();
	~SystemInfo();

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}

#endif
