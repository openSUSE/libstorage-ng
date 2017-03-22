/**
 * File: Diff.cc
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


#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "storage/Utils/Diff.h"

#define VERBOSE 0

using std::cout;
using std::endl;

using namespace storage;


string_vec Diff::diff( const string_vec & lines_a,
		       const string_vec & lines_b,
		       int		  context_lines )
{
    Diff d( lines_a, lines_b, context_lines );

    return d.format_hunks();
}


Diff::Diff( const string_vec & lines_a,
	    const string_vec & lines_b,
	    int		       context_lines ):
    lines_a( lines_a ),
    lines_b( lines_b ),
    context_lines( context_lines )
{
    Range a( lines_a );
    Range b( lines_b );

    diff( a, b );
    fix_hunk_overlap();
}


const Diff::Hunk &
Diff::get_hunk( int index ) const
{
    if ( index < 0 || index >= (int) hunks.size() )
	throw std::out_of_range( "hunk index out of range" );

    return hunks[ index ];
}


void Diff::diff( Range a, Range b )
{
#if VERBOSE
    cout << "diff a.start: " << a.start << " a.end: " << a.end
	 << " b.start: " << b.start << " b.end: " << b.end
	 << endl;
#endif

    // Skip common lines at the start

    while ( a.start <= a.end && b.start <= b.end &&
	    lines_a[ a.start ] == lines_b[ b.start ] )
    {
#if VERBOSE
	cout << "  common start> " << lines_a[ a.start ] << endl;
#endif
	++a.start;
	++b.start;
    }

    // Skip common lines at the end

    while ( a.end > a.start && b.end > b.start &&
	    lines_a[ a.end ] == lines_b[ b.end ] )
    {
#if VERBOSE
	cout << "  common   end> " << lines_a[ a.end ] << endl;
#endif
	--a.end;
	--b.end;
    }


    // Check if there is a diff at all

    if ( a.start <= a.end || b.start <= b.end )
    {
	// Find longest common subsequence

        SubSequence seq = find_common_subsequence( a, b );

	if ( seq.len > 0 )
	{
	    // Cut in two parts and recurse

	    diff( Range( a.start, seq.pos_a - 1 ),
		  Range( b.start, seq.pos_b - 1 ) );

	    diff( Range( seq.pos_a + seq.len, a.end ),
		  Range( seq.pos_b + seq.len, b.end ) );
	}
	else
	{
	    // Hunk output

	    Hunk hunk;

	    add_lines( hunk.lines_removed, lines_a, a );
	    add_lines( hunk.lines_added,   lines_b, b );

	    hunk.removed_start_pos = a.start;
	    hunk.added_start_pos   = b.start;

	    // Add context

	    if ( context_lines > 0 )
	    {
		Range context;
		context.start = std::max( 0, a.start - context_lines );
		context.end   = std::max( 0, a.start - 1 );
		add_lines( hunk.context_lines_before, lines_a, context );

		context.start = std::min( (int) lines_a.size(), a.end + 1 );
		context.end   = std::min( (int) lines_a.size(), a.end + context_lines );
		add_lines( hunk.context_lines_after, lines_a, context );
	    }

	    hunks.push_back( hunk );
	}
    }
}


Diff::SubSequence
Diff::find_common_subsequence( const Range & a, const Range & b )
{
    SubSequence seq;
    int best_len = 0;

    for ( int pos_a = a.start; pos_a <= a.end; ++pos_a )
    {
	const string & start_line_a = lines_a[ pos_a ];

	for ( int pos_b = b.start; pos_b <= b.end; ++pos_b )
	{
	    if ( start_line_a == lines_b[ pos_b ] ) // Found a sequence start
	    {
		int i = pos_a + 1;
		int j = pos_b + 1;

		while ( i <= a.end && j <= b.end &&
			lines_a[ i ] == lines_b[ j ] )
		{
		    ++i;
		    ++j;
		}

		int len = i - pos_a;

		if ( len > best_len ) // This sequence is longer than the old one
		{
		    // Save the sequence we just found

		    best_len  = len;

                    seq.len   = len;
                    seq.pos_a = pos_a;
                    seq.pos_b = pos_b;
		}
	    }
	}
    }

#if VERBOSE
    for ( int i =0; i < best_len; ++i )
        cout << "  common seq> " << lines_a[ seq.pos_a + i ] << endl;
#endif

    return seq;
}


void Diff::add_lines( string_vec &	 lines,
		      const string_vec & lines_to_add )
{
    add_lines( lines, lines_to_add, Range( lines_to_add ) );
}


void Diff::add_lines( string_vec &	 lines,
		      const string_vec & lines_to_add,
		      const Range &	 range )
{
    for ( int i = range.start; i <= range.end; ++i )
	lines.push_back( lines_to_add[i] );
}


void Diff::fix_hunk_overlap()
{
    for ( size_t i=1; i < hunks.size(); ++i )
    {
        Hunk & prev_hunk = hunks[ i-1 ];
        Hunk & hunk      = hunks[ i   ];

        int prev_end = prev_hunk.removed_range().end;
        int current_start = hunk.removed_range().start;
        int overlap = prev_end - current_start + 1;

        string_vec & prev_context    = prev_hunk.context_lines_after;
        string_vec & current_context = hunk.context_lines_before;

        while ( overlap > 0 && prev_context.size() + current_context.size() > 0 )
        {
            if ( ! prev_context.empty() )
            {
                prev_context.pop_back();
                --overlap;
            }

            if ( overlap > 0 && ! current_context.empty() )
            {
                current_context.erase( current_context.begin() );
                --overlap;
            }
        }
    }
}


string_vec Diff::format_hunks()
{
    string_vec result;

    string_vec merged_lines;
    Range merged_range_a( -1, -1 );
    Range merged_range_b( -1, -1 );

    for ( size_t i=0; i < hunks.size(); ++i )
    {
        const Hunk & hunk = hunks[i];
        bool merging = false;

        if ( i+1 < hunks.size() )
        {
            int current_end = hunk.removed_range().end;
            int next_start  = hunks[ i+1 ].removed_range().start;

            if ( current_end +1 >= next_start )
                merging = true;
        }

        if ( merging || ! merged_lines.empty() )
        {
            // Prepare to merge this hunk and the next

            Range range_a = hunk.removed_range();
            Range range_b = hunk.added_range();

            if ( merged_lines.empty() )
            {
                merged_range_a = range_a;
                merged_range_b = range_b;
            }
            else
            {
                merged_range_a.end = range_a.end;
                merged_range_b.end = range_b.end;
            }

            add_lines( merged_lines, hunk.format_lines() );
        }

        if ( ! merging )
        {
            if ( ! merged_lines.empty() )
            {
                // Flush pending merged lines

                result.push_back( Hunk::format_header( merged_range_a, merged_range_b ) );
                add_lines( result, merged_lines );
                merged_lines.clear();
            }
            else
            {
                add_lines( result, hunk.format() );
            }
        }
    }

    return result;
}


string_vec Diff::format_patch_header( const string & filename_old,
                                      const string & filename_new )
{
    string_vec result;

    result.push_back( string( "--- " ) + filename_old );
    result.push_back( string( "+++ " ) + filename_new );

    return result;
}




string_vec Diff::Hunk::format() const
{
    string_vec result = format_lines();
    result.insert( result.begin(), format_header() );

    return result;
}


string_vec Diff::Hunk::format_lines() const
{
    string_vec result;

    add_with_prefix( " ", result, context_lines_before );
    add_with_prefix( "-", result, lines_removed );
    add_with_prefix( "+", result, lines_added	);
    add_with_prefix( " ", result, context_lines_after );

    return result;
}


string Diff::Hunk::format_header() const
{
    return format_header( removed_range(), added_range() );
}


string Diff::Hunk::format_header( const Range & a, const Range & b )
{
    char hunk_header[100];

    snprintf( hunk_header, sizeof( hunk_header ),
	      "@@ -%d,%d +%d,%d @@",
	      a.start + 1, a.length(),
	      b.start + 1, b.length() );

    return string( hunk_header );
}


void Diff::Hunk::add_with_prefix( const string &     prefix,
                                  string_vec &	     lines,
                                  const string_vec & lines_to_add ) const
{
    for ( size_t i=0; i < lines_to_add.size(); ++i )
	lines.push_back( prefix + lines_to_add[i] );
}


Diff::Range
Diff::Hunk::removed_range() const
{
    Range range;

    range.start = removed_start_pos - context_lines_before.size();
    range.end   = removed_start_pos + lines_removed.size() + context_lines_after.size() - 1;

    return range;
}


Diff::Range
Diff::Hunk::added_range() const
{
    Range range;

    range.start = added_start_pos - context_lines_before.size();
    range.end   = added_start_pos + lines_added.size() + context_lines_after.size() - 1;

    return range;
}
