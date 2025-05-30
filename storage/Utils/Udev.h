/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2024] SUSE LLC
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


#ifndef STORAGE_UDEV_H
#define STORAGE_UDEV_H


#include <string>


namespace storage
{
    using std::string;


    string udev_encode(const string&);
    string udev_decode(const string&);

    void udev_settle();


    class Udevadm
    {

    public:

	/**
	 * Settle iff flag is set.
	 */
	void settle();

	void set_settle_needed();

    private:

	bool settle_needed = true;

    };

}


#endif
