/**
 * File: Diff.h
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


#ifndef Diff_h
#define Diff_h

#include <string>
#include <vector>

#define DEFAULT_CONTEXT_LINES   3


namespace storage
{

using std::string;
using std::vector;

typedef vector<string> string_vec;


/**
 * Class to diff string vectors against each other.
 **/
class Diff
{
    /**
     * Helper class defining an interval to operate in: From line no. 'start'
     * (starting with 0) including to line no. 'end'.
     **/
    class Range
    {
    public:
        Range():
            start(0),
            end(-1)
            {}

        Range( int start, int end ):
            start( start ),
            end( end )
            {}

        Range( const string_vec & lines ):
            start(0),
            end( lines.size() - 1 )
            {}

        int  length() const { return end - start + 1; }
        int  size()   const { return length(); }
        bool empty()  const { return length() < 1; }

        int start;
        int end;
    };


    /**
     * Helper class to collect information about one diff 'hunk'.
     *
     * One hunk is one set of changes with a number of consecutive lines
     * removed and a number of consecutive lines added instead.
     **/
    class Hunk
    {
    public:
        string_vec lines_removed;
        string_vec lines_added;
        int        removed_start_pos; // without context lines
        int        added_start_pos;   // without context lines

        string_vec context_lines_before;
        string_vec context_lines_after;

        Hunk():
            removed_start_pos(0),
            added_start_pos(0)
            {}

        /**
         * Format this hunk just like a 'diff -u' output, including the '@@'
         * header.
         **/
        string_vec format() const;

        /**
         * Format the header of this hunk just like the '@@' line in a 'diff
         * -u' output.
         **/
        string format_header() const;

        /**
         * Static version of format_header().
         **/
        static string format_header( const Range & a, const Range & b );

        /**
         * Format the lines of this hunk: Context lines before, removed lines,
         * added lines, context lines after.
         **/
        string_vec format_lines() const;

        /**
         * Return the range of the removed lines including context.
         **/
        Range removed_range() const;

        /**
         * Return the range of the added lines including context.
         **/
        Range added_range() const;

    protected:

        /**
         * Add (append) all lines from 'lines_to_add' to 'lines' and prefix
         * each of the added lines with 'prefix'.
         **/
        void add_with_prefix( const string &     prefix,
                              string_vec &       lines,
                              const string_vec & lines_to_add ) const;
    };


public:

    /**
     * Generic diff method for string vectors: Diff the lines in 'new_lines'
     * against the lines in 'old_lines'.
     *
     * The result is similar to the Linux/Unix "diff -u" command.
     **/
    static string_vec diff( const string_vec & old_lines,
                            const string_vec & new_lines,
                            int context_lines = DEFAULT_CONTEXT_LINES );

    /**
     * Add (append) all lines from 'lines_to_add' to 'lines'.
     **/
    static void add_lines( string_vec &       lines,
                           const string_vec & lines_to_add );

    /**
     * Add (append) a range of lines from 'lines_to_add' to 'lines'.
     **/
    static void add_lines( string_vec &       lines,
                           const string_vec & lines_to_add,
                           const Range &      range );


    /**
     * Constructor. In most cases, it is advised to use the static methods
     * rather than creating your own instance.
     **/
    Diff( const string_vec & lines_a,
          const string_vec & lines_b,
          int                context_lines = DEFAULT_CONTEXT_LINES );

    /**
     * Return the number of result hunks.
     **/
    int get_hunk_count() const { return hunks.size(); }

    /**
     * Return one result hunk.
     **/
    const Hunk & get_hunk( int index ) const;

    /**
     * Format the collected hunks and return them in as a string vector.
     **/
    string_vec format_hunks();

    /**
     * Format a patch header like expected by the Linux patch(1) command:
     *
     *   --- filename_old
     *   +++ filename_new
     *
     * This does not make the diff output prettier, but it can be fed directly
     * to the 'patch' command. If there is no such header, the 'patch' command
     * will complain "input contains only garbage".
     **/
    static string_vec format_patch_header( const string & filename_old,
                                           const string & filename_new );


protected:
    /**
     * Diff lines betwen start and end and add the result to the internal
     * hunks.
     **/
    void diff( Range a, Range b );

    /**
     * Helper struct for the find_common_subsequence return values.
     **/
    struct SubSequence
    {
        int pos_a;
        int pos_b;
        int len;

        SubSequence():
            pos_a(-1),
            pos_b(-1),
            len(0)
            {}
    };

    /**
     * Diff helper: find longest common subsequence between start and
     * end. Return the length of the sequence. Set the _ret parameters to the
     * respective position where the sequence was found.
     **/
    SubSequence find_common_subsequence( const Range & a, const Range & b );

    /**
     * Make sure hunks don't overlap because of context lines.
     **/
    void fix_hunk_overlap();


    //
    // Data members
    //

    const string_vec & lines_a;
    const string_vec & lines_b;
    int                context_lines;
    vector<Hunk>       hunks;
};

} // namespace storage

#endif // Diff_h
