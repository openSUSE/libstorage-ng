/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2017] SUSE LLC
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

#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "storage/EtcCrypttab.h"
#include "storage/Utils/LoggerImpl.h"


#define CRYPTTAB_MIN_COLUMN_COUNT	2
#define CRYPTTAB_MAX_COLUMN_COUNT	4


namespace storage
{
    string CryptOpts::format() const
    {
	string result;

	for ( size_t i=0; i < size(); ++i )
	{
	    if ( ! result.empty() )
		result += ",";

	    result += get_opt(i);
	}

	return result;
    }


    bool CryptOpts::parse( const string & opt_string, int line_no )
    {
	boost::split( opts,
		      opt_string,
		      boost::is_any_of( "," ),
		      boost::token_compress_on );

        return true;
    }




    CrypttabEntry::CrypttabEntry()
    {

    }


    CrypttabEntry::CrypttabEntry( const string & crypt_device,
                                  const string & block_device ):
        crypt_device( crypt_device ),
	block_device( block_device )
    {

    }


    CrypttabEntry::~CrypttabEntry()
    {

    }


    void CrypttabEntry::populate_columns()
    {
	set_column_count( CRYPTTAB_MIN_COLUMN_COUNT );

	set_column( 0, crypt_device );
	set_column( 1, block_device );

        if ( ! password.empty() || ! crypt_opts.empty() )
            add_column( password.empty() ? "none" : password );

        if ( ! crypt_opts.empty() )
            add_column( crypt_opts.format() );
    }


    bool CrypttabEntry::parse( const string & line, int line_no )
    {
	// Delegate splitting into fields to parent class.
	//
	// This always returns 'true' (success), so there is no need to check
	// the result and possibly log an error.

	ColumnConfigFile::Entry::parse( line, line_no );

        int columns = get_column_count();

	if ( columns < CRYPTTAB_MIN_COLUMN_COUNT ||
             columns > CRYPTTAB_MAX_COLUMN_COUNT   )
	{
	    y2err( "crypttab:" << line_no << " Error: wrong number of fields: \"" << line << "\"" );
	    return false;
	}

	int col = 0;
	crypt_device = get_column( col++ );
        block_device = get_column( col++ );

        if ( col < columns )
        {
            password = get_column( col++ );

            if ( password == "none" || password == "-" )
                password = "";
        }

        if ( col < columns )
            crypt_opts.parse( get_column( col++ ), line_no );

	return true; // success
    }




    EtcCrypttab::EtcCrypttab():
	ColumnConfigFile()
    {
	// Set reasonable field widths for /etc/fstab

	int col = 0;
	set_max_column_width( col++, 15 ); // crypt device
	set_max_column_width( col++, 45 ); // block device; just enough for UUID=...
	set_max_column_width( col++, 15 ); // password
	set_max_column_width( col++, 30 ); // crypt options

	set_pad_columns( true );
    }


    EtcCrypttab::~EtcCrypttab()
    {

    }


    CrypttabEntry * EtcCrypttab::find_crypt_device( const string & crypt_device ) const
    {
	for ( int i=0; i < get_entry_count(); ++i )
	{
	    CrypttabEntry * entry = get_entry( i );

	    if ( entry && entry->get_crypt_device() == crypt_device )
		return entry;
	}

	return 0;
    }


    CrypttabEntry * EtcCrypttab::find_block_device( const string & block_device ) const
    {
	for ( int i=0; i < get_entry_count(); ++i )
	{
	    CrypttabEntry * entry = get_entry( i );

	    if ( entry && entry->get_block_device() == block_device )
		return entry;
	}

	return 0;
    }


    CrypttabEntry * EtcCrypttab::get_entry( int index ) const
    {
        ST_CHECK_INDEX( index, 0, get_entry_count() - 1 );

        CommentedConfigFile::Entry * entry =
            CommentedConfigFile::get_entry( index );

        ST_CHECK_PTR( entry );

        return dynamic_cast<CrypttabEntry *>( entry );
    }


    void EtcCrypttab::log()
    {
        string_vec lines = format_lines();

        if ( ! get_filename().empty() )
            y2mil( get_filename() );

        for ( size_t i=0; i < lines.size(); ++i )
            y2mil( lines[i] );
    }

}
