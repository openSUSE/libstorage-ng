/*
 * File: CommentedConfigFile.cc
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


#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/CommentedConfigFile.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/AsciiFile.h"

using std::cout;
using std::endl;

#define WHITESPACE " \t"


using namespace storage;


CommentedConfigFile::CommentedConfigFile():
    comment_marker( "#" ),
    diff_enabled( false )
{
}


CommentedConfigFile::~CommentedConfigFile()
{
    clear_entries();
}


CommentedConfigFile::Entry * CommentedConfigFile::get_entry( int index ) const
{
    if ( index < 0 || index >= (int) entries.size() )
        return 0;
    else
        return entries[ index ];
}


int CommentedConfigFile::get_index_of( const Entry * wanted_entry ) const
{
    for ( size_t i=0; i < entries.size(); ++i )
    {
	if ( entries[i] == wanted_entry )
	    return i;
    }

    return -1;
}


CommentedConfigFile::Entry * CommentedConfigFile::take( int index )
{
    if ( index < 0 || index >= (int) entries.size() )
        return 0;

    Entry * entry = entries[ index ];
    entries.erase( entries.begin() + index );
    entry->set_parent( 0 );

    return entry;
}


void CommentedConfigFile::remove( int index )
{
    Entry * entry = take( index );

    if ( entry )
        delete entry;
}


void CommentedConfigFile::remove( Entry * entry )
{
    if ( ! entry )
        return;

    int index = get_index_of( entry );

    if ( index != -1 )
        remove( index );
}


void CommentedConfigFile::insert( int before, Entry * entry )
{
    entries.insert( entries.begin() + before, entry );
    entry->set_parent( this );
}


void CommentedConfigFile::append( Entry * entry )
{
    entries.push_back( entry );
    entry->set_parent( this );
}


bool CommentedConfigFile::read( const string & filename )
{
    this->filename = filename;

    // reading is done via AsciiFile to the mockup playback and recording

    AsciiFile ascii_file(filename);
    bool success = parse(ascii_file.get_lines());

    return success;
}


bool CommentedConfigFile::write( const string & new_filename )
{
    string name = new_filename;

    if ( new_filename.empty() )
        name = this->filename;
    else
        this->filename = name;

    if ( name.empty() ) // Support for mocking:
        return true;    // Pretend everything worked just fine.

    std::ofstream file( name, std::ofstream::out | std::ofstream::trunc );

    if ( ! file.is_open() )
        return false;

    string_vec lines = format_lines();

    for ( size_t i=0; i < lines.size(); ++i )
        file << lines[i] << "\n"; // no endl: Don't flush after every line

    return true;
}


bool CommentedConfigFile::parse( const string_vec & lines )
{
    clear_all();

    int header_end    = find_header_comment_end( lines );
    int footer_start  = find_footer_comment_start( lines, header_end + 1 );
    int content_start = 0;
    int content_end   = lines.size() - 1;

    if ( header_end > -1 )
    {
        for ( int i=0; i <= header_end; ++i )
            header_comments.push_back( lines[i] );

        content_start = header_end + 1;
    }

    if ( footer_start > -1 )
    {
        for ( size_t i = footer_start; i < lines.size(); ++i )
            footer_comments.push_back( lines[i] );

        content_end = footer_start - 1;
    }

    bool success = parse_entries( lines, content_start, content_end );

    if ( diff_enabled )
        save_orig();

    return success;
}


bool CommentedConfigFile::parse_entries( const string_vec & lines,
                                         int from,
                                         int end )
{
    string_vec comment_before;
    bool success;

    for ( int i = from; i <= end; ++i )
    {
        const string & line = lines[i];

        if ( is_empty_line( line ) || is_comment_line( line ) )
            comment_before.push_back( line );
        else // found a content line
        {
            CommentedConfigFile::Entry * entry = create_entry();
            ST_CHECK_PTR( entry );

            entry->set_comment_before( comment_before );
            comment_before.clear();
            string content;
            string line_comment;
            split_off_comment( line, content, line_comment );
            entry->set_line_comment( line_comment );
            bool ok = entry->parse( content, i+1 );

            if ( ok )
                append( entry );
            else
            {
                success = false;
                delete entry;
            }
        }
    }

    return success;
}


int CommentedConfigFile::find_header_comment_end( const string_vec & lines )
{
    int header_end      = -1;
    int last_empty_line = -1;

    for ( int i=0; i < (int) lines.size(); ++i )
    {
        const string & line = lines[i];

        if ( is_empty_line( line ) )
            last_empty_line = i;
        else if ( is_comment_line( line ) )
            header_end = i;
        else // found the first content line
            break;
    }

    if ( last_empty_line > 0 )
    {
        header_end = last_empty_line;

        // This covers two cases:
        //
        // - If there were empty lines and no more comment lines before the
        //   first content line, the empty lines belong to the header comment.
        //
        // - If there were empty lines and then some more comment lines before
        //   the first content line, the comments after the last empty line no
        //   longer belong to the header comment, but to the first content
        //   entry. So let's go back to that last empty line.
    }

    return header_end;
}


int CommentedConfigFile::find_footer_comment_start( const string_vec & lines,
                                                    int from )
{
    int footer_start = -1;

    for ( int i = lines.size()-1; i >= from; --i )
    {
        const string & line = lines[i];

        if ( is_empty_line( line ) || is_comment_line( line ) )
            footer_start = i;
        else // found the last content line
            break;
    }

    return footer_start;
}


string_vec CommentedConfigFile::format_lines()
{
    string_vec lines = header_comments;

    for ( size_t i=0; i < entries.size(); ++i )
    {
        Entry * entry = entries[i];

        for ( size_t j=0; j < entry->get_comment_before().size(); ++j )
            lines.push_back( entry->get_comment_before()[j] );

        string line = entry->format();

        if ( ! entry->get_line_comment().empty() )
            line += " " + entry->get_line_comment();

        lines.push_back( line );
    }

    for ( size_t i=0; i < footer_comments.size(); ++i )
        lines.push_back( footer_comments[i] );

    return lines;
}


void CommentedConfigFile::clear_entries()
{
    for ( size_t i=0; i < entries.size(); ++i )
	delete entries[i];

    entries.clear();
}


void CommentedConfigFile::clear_all()
{
    clear_entries();
    header_comments.clear();
    footer_comments.clear();
}


bool CommentedConfigFile::is_comment_line( const string & line )
{
    size_t pos = line.find_first_not_of( WHITESPACE );

    if ( pos == string::npos ) // No non-whitespace character in line
        return false;

    if ( pos == 0 )
        return boost::starts_with( line, comment_marker );
    else
        return boost::starts_with( line.substr( pos ), comment_marker );
}


bool CommentedConfigFile::is_empty_line( const string & line )
{
    if ( line.empty() )
        return true;

    return line.find_first_not_of( WHITESPACE ) == string::npos;
}


void CommentedConfigFile::split_off_comment( const string & line,
					     string & content_ret,
					     string & comment_ret )
{
    size_t pos = line.find( comment_marker );

    if ( pos == string::npos )
    {
        comment_ret = "";
        content_ret = line;
    }
    else
    {
        content_ret = line.substr( 0, pos-1 );
        comment_ret = line.substr( pos );
    }

    strip_trailing_whitespace( content_ret );
}


void CommentedConfigFile::strip_trailing_whitespace( string & line )
{
    size_t pos = line.find_last_not_of( WHITESPACE );

    if ( pos != string::npos )
        line = line.substr( 0,  pos+1 );
    else
        line.clear();
}


string_vec CommentedConfigFile::diff()
{
    return diff( orig_lines, format_lines() );;
}


string_vec CommentedConfigFile::diff( const string_vec & formatted_lines )
{
    return diff( orig_lines, formatted_lines );
}


void CommentedConfigFile::save_orig()
{
    save_orig( format_lines() );
}


void CommentedConfigFile::save_orig( const string_vec & new_orig_lines )
{
    orig_lines = new_orig_lines;
}


string_vec CommentedConfigFile::diff( const string_vec & old_lines,
                                      const string_vec & new_lines )
{
    return diff( old_lines, 0, old_lines.size() - 1,
                 new_lines, 0, new_lines.size() - 1 );
}


string_vec CommentedConfigFile::diff( const string_vec & old_lines,
                                      int old_start, int old_end,
                                      const string_vec & new_lines,
                                      int new_start, int new_end )
{
    string_vec diff_lines;

    // Skip common lines at the start

    while ( old_start <= old_end && new_start <= new_end &&
            old_lines[ old_start ] == new_lines[ new_start ] )
    {
        ++old_start;
        ++new_start;
    }

    // Skip common lines at the end

    while ( old_end > old_start && new_end > new_start &&
            old_lines[ old_end ] == new_lines[ new_end ] )
    {
        --old_end;
        --new_end;
    }


    // Check if there is a diff at all

    if ( old_start <= old_end || new_start <= new_end )
    {
        // Find longest common subsequence

        int common_pos_old = old_start;
        int common_pos_new = new_start;
        int common_len = 0;

        common_len = find_common_subsequence( old_lines, old_start, old_end,
                                              new_lines, new_start, new_end,
                                              common_pos_old, common_pos_new );

        if ( common_len > 0 )
        {
            // Cut in two parts and recurse

            string_vec chunk1 =
                diff( old_lines, old_start, common_pos_old - 1,
                      new_lines, new_start, common_pos_new - 1 );

            string_vec chunk2 =
                diff( old_lines, common_pos_old + common_len, old_end,
                      new_lines, common_pos_new + common_len, new_end );

            for ( size_t i=0; i < chunk1.size(); ++i )
                diff_lines.push_back( chunk1[i] );

            for ( size_t i=0; i < chunk2.size(); ++i )
                diff_lines.push_back( chunk2[i] );
        }
        else
        {
            // Chunk output

            char chunk_header[255];
            sprintf( chunk_header, "@@ -%d,%d +%d,%d @@",
                     old_start + 1, old_end - old_start + 1,
                     new_start + 1, new_end - new_start + 1);

            diff_lines.push_back( string( chunk_header ) );


            while ( old_start <= old_end )
                diff_lines.push_back( string( "-" ) + old_lines[ old_start++ ] );

            while ( new_start <= new_end )
                diff_lines.push_back( string( "+" ) + new_lines[ new_start++ ] );
        }
    }

    return diff_lines;
}


int CommentedConfigFile::find_common_subsequence( const string_vec old_lines,
                                                  int old_start, int old_end,
                                                  const string_vec & new_lines,
                                                  int new_start, int new_end,
                                                  int & common_pos_old_ret,
                                                  int & common_pos_new_ret )
{
    common_pos_old_ret = old_end + 1;
    common_pos_new_ret = new_end + 1;
    int best_len = 0;

    for ( int old_pos = old_start; old_pos <= old_end; ++old_pos )
    {
        const string & old_start_line = old_lines[ old_pos ];

        for ( int new_pos = new_start; new_pos <= new_end; ++new_pos )
        {
            if ( old_start_line == new_lines[ new_pos ] ) // Found a sequence start
            {
                int i = old_pos + 1;
                int j = new_pos + 1;

                while ( i <= old_end && j <= new_end &&
                        old_lines[ i ] == new_lines[ j ] )
                {
                    ++i;
                    ++j;
                }

                int len = i - old_pos;

                if ( len > best_len ) // This sequence is longer than the old one
                {
                    // Save the sequence we just found

                    best_len = len;
                    common_pos_old_ret = old_pos;
                    common_pos_new_ret = new_pos;
                }
            }
        }
    }

    return best_len;
}
