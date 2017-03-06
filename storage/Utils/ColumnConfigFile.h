/*
 * File: ColumnConfigFile.h
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


#ifndef ColumnConfigFile_h
#define ColumnConfigFile_h

#include "storage/Utils/CommentedConfigFile.h"


namespace storage
{

/**
 * Utility class to read and write column-oriented config files that might
 * contain comments that should be preserved when writing the file.
 *
 * One example for this would be /etc/fstab:
 *
 *    # /etc/fstab
 *    #
 *    # <file system>	<mount point>  <type> <options>	    <dump> <pass>
 *
 *    /dev/disk/by-label/swap	 none	swap  sw		 0  0
 *    /dev/disk/by-label/Ubuntu	 /	ext4  errors=remount-ro	 0  1
 *    /dev/disk/by-label/work	 /work	ext4  defaults		 0  2
 *
 * There are 6 columns, separated by whitespace. This class is a refinement of
 * the more generic CommentedConfigFile class to handle such cases.
 **/
class ColumnConfigFile: public CommentedConfigFile
{
public:

    class Entry: public CommentedConfigFile::Entry
    {
    public:
	Entry() {}
	virtual ~Entry() {}

	/**
	 * Format the content as a string.
	 *
	 * Reimplemented from CommentedConfigFile.
	 **/
	virtual string format();

	/**
	 * Parse a content line. Return 'true' on success, 'false' on error.
	 *
	 * Reimplemented from CommentedConfigFile.
	 **/
	virtual bool parse( const string & line );

	/**
	 * Return the number of columns for this entry.
	 **/
	int get_column_count() const { return columns.size(); }

	/**
	 * Return one of the columns for this entry.
	 **/
	const string & get_column( int i ) const { return columns[i]; }

	/**
	 * Set a new value for column no. 'i'.
	 **/
	void set_column( int i, const string & new_value )
	    { columns[i] = new_value; }

	void add_column( const string & new_value )
	    { columns.push_back( new_value ); }

    protected:

	/**
	 * Split a line into fields and return them.
	 **/
	virtual string_vec split( const string & line ) const;

    private:

	string_vec columns;
    };


    ColumnConfigFile();
    virtual ~ColumnConfigFile();

    /**
     * Factory method to create one entry.
     *
     * Reimplemented from CommentedConfigFile.
     **/
    virtual Entry * create_entry() { return new ColumnConfigFile::Entry(); }

    /**
     * Format the entire file as string lines, including header, footer and all
     * other comments.
     *
     * Reimplemented from CommentedConfigFile.
     **/
    virtual string_vec format_lines();

    /**
     * Return 'true' if columns should be padded upon output, i.e. they should
     * get the same widths so they neatly line up. The default is 'true'.
     *
     * Padding is limited by maximum column widths which can be set globally or
     * per column with set_max_column_width().
     **/
    bool get_pad_columns() const { return pad_columns; }

    /**
     * Enable or disable column padding.
     **/
    void set_pad_columns( bool do_pad ) { pad_columns = do_pad; }

    /**
     * Return the best column width for a column. If a maximum width for this
     * column is set, this returns no more than the maximum width.
     **/
    virtual int get_column_width( int column );

    /**
     * Get the maximum column width for a column. 0 means unlimited.
     **/
    virtual int get_max_column_width( int column );

    /**
     * Set the maximum column width for a column. 0 means unlimited.
     **/
    void set_max_column_width( int column, int new_size );

    /**
     * Set a maximum column width for all columns. 0 means unlimited.
     **/
    void set_max_column_width( int new_size );


protected:

    void calc_column_widths();

    vector<int> column_widths;
    vector<int> max_column_widths;
    int         max_column_width;
    bool        pad_columns;
};

} // namespace storage    

#endif // ColumnConfigFile_h
