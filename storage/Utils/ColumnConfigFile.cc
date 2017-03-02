/*
 * File: ColumnConfigFile.cc
 *
 * Copyright (c) 2017 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *
 * This is part of the commented-config-file project.
 * License: GPL V2
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
 */


#include <iostream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "storage/Utils/ColumnConfigFile.h"


#define WHITESPACE                      " \t"
#define DEFAULT_MAX_COLUMN_WIDTH        40


using namespace storage;


string ColumnConfigFile::Entry::format()
{
    string result;

    for ( size_t i=0; i < columns.size(); ++i )
    {
        if ( ! result.empty() )
            result += "  ";

        string col = columns[i];

        if ( get_parent() )
        {
            ColumnConfigFile * col_parent = dynamic_cast<ColumnConfigFile *>( get_parent() );

            if ( col_parent && col_parent->get_pad_columns() )
            {
                size_t field_width = col_parent->get_column_width( i );

                if ( col.size() < field_width ) // Pad to desired width
                    col += string( field_width - col.size(), ' ' );
            }
        }

        result += col;
    }

    return result;
}


bool ColumnConfigFile::Entry::parse( const string & line, int line_no )
{
    (void) line_no;

    set_content( line );
    columns = split( line );

    return true;
}


string_vec ColumnConfigFile::Entry::split( const string & line ) const
{
    string_vec fields;

    boost::split( fields,
                  line,
                  boost::is_any_of( WHITESPACE ),
                  boost::token_compress_on );

    return fields;
}




ColumnConfigFile::ColumnConfigFile():
    CommentedConfigFile(),
    max_column_width( DEFAULT_MAX_COLUMN_WIDTH ),
    pad_columns( true )
{

}


ColumnConfigFile::~ColumnConfigFile()
{

}


string_vec ColumnConfigFile::format_lines()
{
    calc_column_widths();

    return CommentedConfigFile::format_lines();
}


int ColumnConfigFile::get_column_width( int column )
{
    if ( column_widths.empty() )
        calc_column_widths();

    if ( column < 0 || column >= (int) column_widths.size() )
        return 0;
    else
        return column_widths[ column ];
}


int ColumnConfigFile::get_max_column_width( int column )
{
    if ( column < 0 || column >= (int) max_column_widths.size() )
        return max_column_width;
    else
        return max_column_widths[ column ];
}


void ColumnConfigFile::set_max_column_width( int column, int new_size )
{
    if ( column >= (int) max_column_widths.size() )
        max_column_widths.resize( column+1 );

    max_column_widths[ column ] = new_size;
}


void ColumnConfigFile::calc_column_widths()
{
    column_widths.clear();

    if ( ! pad_columns )
        return;

    int columns = 0;

    for ( int i=0; i < get_entry_count(); ++i )
    {
        ColumnConfigFile::Entry * entry =
            dynamic_cast<ColumnConfigFile::Entry*>( get_entry( i ) );

        if ( entry )
        {
            entry->populate_columns();
            columns = std::max( entry->get_column_count(), columns );
        }
    }

    column_widths.resize( columns );

    for ( int i=0; i < get_entry_count(); ++i )
    {
        ColumnConfigFile::Entry * entry =
            dynamic_cast<ColumnConfigFile::Entry*>( get_entry( i ) );

        if ( entry )
        {
            for ( int col=0; col < entry->get_column_count(); ++col )
            {
                int width = entry->get_column( col ).size();
                int max   = get_max_column_width( col );

                // Only take the width of this column of this entry into
                // account if it is not wider than the maximum for this column;
                // otherwise we will always end up with the maximum width for
                // any column that has just one item the maximum width, but for
                // that one item the maximum will be exceeded anyway (otherwise
                // we'd have to cut if off which we clearly can't). So oversize
                // column items should not be part of this calculation; we want
                // to know the widths of the "normal" items only.

                if ( max == 0 || width <= max )
                    column_widths[ col ] = std::max( column_widths[ col ], width );
            }
        }
    }

#if 0
    for ( int col=0; col < columns; ++col )
        std::cout << "Col " << col << " width: " << column_widths[col] << std::endl;
#endif
}


ColumnConfigFile::Entry * ColumnConfigFile::get_entry( int index ) const
{
    CommentedConfigFile::Entry * entry = get_entry( index );

    return entry ? dynamic_cast<ColumnConfigFile::Entry *>( entry ) : 0;
}
