/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#ifndef STORAGE_HUMAN_STRING_H
#define STORAGE_HUMAN_STRING_H


#include <string>


namespace storage
{

    // TODO use C++11 user-defined literals? Swig has some support for them.


    /**
     * Abbreviation for byte.
     *
     * @see https://en.wikipedia.org/wiki/Byte
     */
    const unsigned long long B = 1;

    /**
     * Abbreviation for kibibyte.
     *
     * @see https://en.wikipedia.org/wiki/Kibibyte
     */
    const unsigned long long KiB = 1024 * B;

    /**
     * Abbreviation for mebibyte.
     *
     * @see https://en.wikipedia.org/wiki/Mebibyte
     */
    const unsigned long long MiB = 1024 * KiB;

    /**
     * Abbreviation for gibibyte.
     *
     * @see https://en.wikipedia.org/wiki/Gibibyte
     */
    const unsigned long long GiB = 1024 * MiB;

    /**
     * Abbreviation for tebibyte.
     *
     * @see https://en.wikipedia.org/wiki/Tebibyte
     */
    const unsigned long long TiB = 1024 * GiB;

    /**
     * Abbreviation for pebibyte.
     *
     * @see https://en.wikipedia.org/wiki/Pebibyte
     */
    const unsigned long long PiB = 1024 * TiB;

    /**
     * Abbreviation for exbibyte.
     *
     * @see https://en.wikipedia.org/wiki/Exbibyte
     */
    const unsigned long long EiB = 1024 * PiB;


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
     * B, KiB, MiB, GiB, TiB, PiB or EiB as unit as appropriate. Supported
     * range is 0 B to (16 EiB - 1 B).
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
     * binary. Supported range is 0 B to (16 EiB - 1 B).
     *
     * @param str size string
     * @param classic use classic locale instead of global C++ locale
     * @return bytes
     *
     * The conversion is always case-insensitive. With classic set to
     * false the conversion is also sloppy concerning omission of 'B'.
     *
     * @throw ParseException, OverflowException
     */
    unsigned long long humanstring_to_byte(const std::string& str, bool classic);

}


#endif
