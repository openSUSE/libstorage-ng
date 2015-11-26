/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#ifndef STORAGE_HUMAN_STRING_H
#define STORAGE_HUMAN_STRING_H

#include <string>
#include <stdexcept>

#include "storage/StorageInterface.h"


namespace storage
{

    using namespace storage_legacy;


    struct ParseError : public std::runtime_error
    {
        ParseError(const std::string& msg) throw() : runtime_error(msg) {}
    };


    /**
     * Return number of suffixes.
     *
     * @return number of suffixes
     */
    int num_suffixes();

    /**
     * Return a suffix.
     *
     * @param i index of suffix
     * @param classic use classic locale instead of global C++ locale
     * @return suffix
     */
    std::string get_suffix(int i, bool classic);


    /**
     * Return a pretty description of a size with required precision and using
     * B, KiB, MiB, GiB, TiB, PiB or EiB as unit as appropriate.
     *
     * @param size size in bytes
     * @param classic use classic locale instead of global C++ locale
     * @param precision number of fraction digits in output
     * @param omit_zeroes if true omit trailing zeroes for exact values
     * @return formatted string
     */
    std::string byte_to_humanstring(unsigned long long size, bool classic, int precision,
				    bool omit_zeroes);

    /**
     * Converts a size description using B, KiB, KB, MiB, MB, GiB, GB, TiB,
     * TB, PiB, PB, EiB or EB into an integer. Decimal suffixes are treated as
     * binary.
     *
     * @param str size string
     * @param classic use classic locale instead of global C++ locale
     * @param size size in bytes
     * @return true on successful conversion
     *
     * The conversion is always case-insensitive. With classic set to
     * false the conversion is also sloppy concerning omission of 'B'.
     */
    unsigned long long humanstring_to_byte(const std::string& str, bool classic);


    std::string get_mount_by_name(MountByType mount_by_type);

    std::string get_transport_name(Transport transport);

}


#endif
