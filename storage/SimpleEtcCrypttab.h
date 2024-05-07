/*
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


#ifndef STORAGE_SIMPLE_ETC_CRYPTTAB_H
#define STORAGE_SIMPLE_ETC_CRYPTTAB_H


#include <string>
#include <vector>


namespace storage
{

    /**
     * A simple struct to hold the information from one line in crypttab. See
     * crypttab(5).
     */
    struct SimpleEtcCrypttabEntry
    {
	std::string name;
	std::string device;
	std::string password;
	std::vector<std::string> crypt_options;
    };


    /**
     * Read a crypttab file.
     *
     * Whether the function returns an empty vector or throws an exception if
     * reading or parsing fails is undefined.
     *
     * @throw Exception
     */
    std::vector<SimpleEtcCrypttabEntry> read_simple_etc_crypttab(const std::string& filename);

}


#endif
