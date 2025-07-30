/*
 * File: CommentedConfigFile.h
 *
 * Copyright (c) 2017 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 * Copyright (c) 2025 SUSE LLC
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


#ifndef CommentedConfigFile_h
#define CommentedConfigFile_h

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>


namespace storage
{

using std::string;
using std::vector;

typedef vector<string> string_vec;


/**
 * Utility class to read and write config files that might contain comments.
 * This class tries to preserve any existing comments and keep them together
 * with the content line immediately following them.
 *
 * This class supports the notion of a header comment block, a footer comment
 * block, a comment block preceding any content line and a line comment on the
 * content line itself.
 *
 * A comment preceding a content line is stored together with the content line,
 * so moving around entries in the file will keep the comment with the content
 * line it belongs to.
 *
 * The default comment marker is '#' like in most Linux config files, but it
 * can be set with setCommentMarker().
 *
 * Example (line numbers added for easier reference):
 *
 *   001    # Header comment 1
 *   002    # Header comment 2
 *   003    # Header comment 3
 *   004
 *   005
 *   006    # Header comment 4
 *   007    # Header comment 5
 *   008
 *   009    # Content line 1 comment 1
 *   010    # Content line 1 comment 2
 *   011    content line 1
 *   012    content line 2
 *   013
 *   014    content line 3
 *   015
 *   016    content line 4
 *   017    content line 5 # Line comment 5
 *   018    # Content line 6 comment 1
 *   019
 *   020    content line 6 # Line comment 6
 *   021    content line 7
 *   022
 *   023    # Footer comment 1
 *   024    # Footer comment 2
 *   025
 *   026    # Footer comment 3
 *
 *
 * Empty lines or lines that have only whitespace belong to the next comment
 * block: The footer comment consists of lines 022..026.
 *
 * The only exception is the header comment that stretches from the start of
 * the file to the last empty line preceding a content line. This is what
 * separates the header comment from the comment that belongs to the first
 * content line. In this example, the header comment consists of lines
 * 001..008.
 *
 * Content line 1 in line 011 has comments 009..010.
 * Content line 2 in line 012 has no comment.
 * Content line 3 in line 014 has comment 013 (an empty line).
 * Content line 5 in line 017 has a line comment "# Line comment 5".
 * Content line 6 in line 020 has comments 018..019 and a line comment.
 *
 * Applications using this class can largely just ignore all the comment stuff;
 * the class will handle the comments automagically.
 **/
class CommentedConfigFile: private boost::noncopyable
{
public:

    /**
     * Class representing one content line and the preceding comments.
     *
     * This class is meant to be subclassed. When you do that, remember to also
     * overwrite the CommentedConfigFile::createEntry() factory method.
     **/
    class Entry
    {
    public:
	/**
	 * Constructor. Not accepting a 'line' argument because that would need
	 * to be parsed, and that would require calling the overloaded virtual
	 * parse() function which is not possible in the constructor.
	 **/
	Entry():
	    parent(0)
	    {}

	/**
	 *
	 **/
	virtual ~Entry() {}

	/**
	 * Format the content as a string.
	 * Derived classes might choose to override this.
	 * Do not add 'line_comment'; it is added automatically.
	 **/
	virtual string format() { return content; }

        /**
         * Validate this entry just prior to formatting/writing it back to
         * file: Return 'true' if okay, 'false' if not.
         *
         * If this returns 'false', the entry is not formatted/written -
         * neither the entry itself nor the comments that belong to it. Error
         * handling such as logging an error or whatever is this function's
         * responsibility.
         *
         * This default implementation always returns 'true'.
         **/
        virtual bool validate() { return true; }

	/**
	 * Parse a content line. Return 'true' on success, 'false' on error.
         * 'line_no' (if >0) is the line number in the current file. This can
         * be used for error reporting.
         *
	 * Derived classes might choose to override this.
	 **/
	virtual bool parse( const string & line, int line_no = -1 )
	    { content = line; return true; }

        /**
         * Return the string content of this entry.
         **/
        const string & get_content() const { return content; }

        /**
         * Set the string content of this entry.
         *
         * This should not normally be necessary; the default parse() function
         * does that implicitly.
         **/
        void set_content( const string & new_content ) { content = new_content; }

        /**
         * Return the comment block before this entry: Empty lines or lines
         * starting with the comment marker ("#") as their first non-whitspace
         * character.
         **/
        const string_vec & get_comment_before() const { return comment_before; }

        /**
         * Set the comment block before this entry.
         **/
        void set_comment_before( const string_vec & new_comment_before )
            { comment_before = new_comment_before; }

        /**
         * Return the comment on the same line as this entry's content.
         *
         * This will usually be an empty string. If it is non-empty, it will
         * start with the comment marker ("#").
         **/
        const string & get_line_comment() const { return line_comment; }

        /**
         * Set the comment on the same line as this entry's comment.
         * This string should start with the comment marker ("#").
         **/
        void set_line_comment( const string & new_comment )
            { line_comment = new_comment; }

        /**
         * Return the Parent CommentConfigFile or 0 if this entry is not
         * currently n a CommentConfigFile's entries.
         **/
        CommentedConfigFile * get_parent() const { return parent; }

        /**
         * Set this entry's CommentedConfigFile parent.
         *
         * This is done automatically when this entry is added to a
         * CommentedConfigFile's entries, and it is set to 0 when it is removed
         * from there (with take() or remove() ).
         *
         * This is meant to be used by the parent CommentedConfigFile only.
         * Use outside of this only if you know what you are doing.
         **/
        void set_parent( CommentedConfigFile * new_parent )
            { parent = new_parent; }

    private:

	//
	// Data members
	//

	string_vec comment_before;
	string	   line_comment;   // at the end of the line
	string	   content;

	CommentedConfigFile* parent = nullptr;
    };


    //----------------------------------------------------------------------


    /**
     * Constructor.
     *
     * This intentionally does not accept a filename to start reading a file
     * right away because that would call virtual methods indirectly that
     * cannot be called in the constructor.
     **/
    CommentedConfigFile(int permissions = DEFAULT_PERMISSIONS);

    /**
     * Destructor. This deletes all entries.
     **/
    virtual ~CommentedConfigFile();


    /**
     * Read 'filename' and replace the current content with it.
     * Return 'true' if success, 'false' if error.
     **/
    bool read( const string & filename );

    /**
     * Write the contents to 'filename' or, if 'filename' is empty, to the
     * original file that was used in the constructor or during the last
     * read().
     *
     * @throw IOException
     **/
    void write( const string & filename = "" );

    /**
     * Parse 'lines' and replace the current content with it.
     **/
    bool parse( const string_vec & lines );

    /**
     * Format the entire file as string lines, including header, footer and all
     * other comments.
     *
     * This is intentionally non-const so derived classes can update any
     * formatting information in an overwritten version of this function
     **/
    virtual string_vec format_lines();

    /**
     * Factory method to create one entry.
     *
     * Derived classes can override this to create their own derived entry
     * class. Ownership of the created object is transferred to this class
     * which will delete it as appropriate.
     **/
    virtual Entry * create_entry() { return new Entry(); }

    /**
     * Return the number of entries.
     **/
    int get_entry_count() const { return entries.size(); }

    /**
     * Return 'true' if there are no entries, 'false' if there are any.
     **/
    bool empty() const { return entries.empty(); }

    /**
     * Return an iterator that points to the first entry.
     **/
    vector<Entry *>::const_iterator begin() const { return entries.begin(); }

    /**
     * Return an iterator that points one element after the last entry.
     **/
    vector<Entry *>::const_iterator end()   const { return entries.end(); }

    /**
     * Return entry no. 'index' or 0 if 'index' is out of range.
     **/
    Entry * get_entry( int index ) const;

    /**
     * Convenience method because it is probably the most used:
     * Return the content of entry no. 'index'.
     **/
    const string & get_content( int index ) { return get_entry( index )->get_content(); }

    /**
     * Clear and delete all entries. This leaves the header and footer comments
     * intact.
     **/
    void clear_entries();

    /**
     * Clear all: entries, header and footer comments.
     **/
    void clear_all();

    /**
     * Return the index of 'entry' or -1 if there is no such entry.
     **/
    int get_index_of( const Entry * entry ) const;

    /**
     * Take the entry with the specified index out of the entries and return
     * it. This is useful when rearranging the order of entries: Take it out
     * and insert it at another place
     *
     * The caller takes over ownership of the entry and has to delete it when
     * appropriate (except of course when it is re-inserted into the entries at
     * a later time, in which case this class will take over ownership again).
     **/
    Entry * take( int index );

    /**
     * Remove the entry with the specified index from the entries and delete it.
     **/
    void remove( int index );

    /**
     * Remove an entry and delete it.
     **/
    void remove( Entry * entry );

    /**
     * Insert 'entry' before index 'before'.
     * This transfers ownership of the entry to this class.
     **/
    void insert( int before, Entry * entry );

    /**
     * Append 'entry' at the end of the entries.
     * This transfers ownership of the entry to this class.
     **/
    void append( Entry * entry );

    /**
     * Very much like append(), but it returns a reference to this instance so
     * the operators can be chained:
     *
     *	   etcFstab << entry_a << entry_b << entry_c;
     **/
    CommentedConfigFile & operator<<( Entry * entry )
	{ append( entry ); return *this; }

    /**
     * Return the header comments (including empty lines).
     **/
    const string_vec & get_header_comments() { return header_comments; }

    /**
     * Set the header comments. Each line should be an empty line or a line
     * with the comment marker ("#") as the first non-whitespace character.
     **/
    void set_header_comments( const string_vec & new_comments )
        { header_comments = new_comments; }

    /**
     * Return the footer comments (including empty lines).
     **/
    const string_vec & get_footer_comments() { return footer_comments; }

    /**
     * Set the footer comments. Each line should be an empty line or a line
     * with the comment marker ("#") as the first non-whitespace character.
     **/
    void set_footer_comments( const string_vec & new_comments )
        { footer_comments = new_comments; }

    /**
     * Get the last filename content was read from. This may be empty.
     **/
    const string & get_filename() const { return filename; }

    /**
     * Return the comment marker (default: "#").
     **/
    const string & get_comment_marker() const { return comment_marker; }

    /**
     * Set the comment marker for subsequent read() and parse() operations.
     **/
    void set_comment_marker( const string & marker ) { comment_marker = marker; }

    /**
     * Return the inline comments setting.
     */
    bool is_inline_comments() const { return inline_comments; }

    /**
     * Set whether comments can be inline or just at the beginning of the line.
     */
    void set_inline_comments(bool value) { inline_comments = value; }

    /**
     * Return 'true' if diffs are enabled. Diffs are not enabled by default.
     **/
    bool is_diff_enabled() const { return diff_enabled; }

    /**
     * Enable or disable diffs. This saves a copy of the formatted text lines
     * at certain points, i.e. this comes at a memory and performance cost.
     **/
    void set_diff_enabled( bool enabled = true ) { diff_enabled = enabled; }

    /**
     * Diff the current status against the last one saved with save_orig().
     **/
    string_vec diff();

    /**
     * Diff 'formatted_lines' against the last status saved with save_orig().
     **/
    string_vec diff( const string_vec & formatted_lines );

    /**
     * Save the current status as the original reference for future diffs.
     * This calls format_lines() internally which is a pretty expensive
     * operation, so if you call format_lines() anyway, consider using the
     * overloaded version of this that takes a string_vec.
     *
     * Notice that this is called automatically when the file is loaded, when
     * lines are parsed and when the file is written.
     **/
    void save_orig();

    /**
     * Save the 'orig_lines' as the original reference for future
     * diffs. 'orig_lines' should be the result of a previous format_lines()
     * call.
     **/
    void save_orig( const string_vec & orig_lines );

    /**
     * Get the orig_lines as saved with the last save_orig().
     **/
    const string_vec & get_orig_lines() const { return orig_lines; }

    /**
     * Generic static diff method: Diff the lines in 'new_lines' against the
     * lines in 'old_lines'.
     *
     * The result is similar to the Linux/Unix "diff -u" command.
     **/
    static string_vec diff( const string_vec & old_lines,
                            const string_vec & new_lines );


protected:

    /**
     * Return 'true' if this is a comment line (not an empty line!), i.e. the
     * first nonblank character is the comment marker ("#" by default).
     **/
    bool is_comment_line( const string & line );

    /**
     * Return 'true' if this is an empty line, i.e. there are no nonblank
     * characters.
     **/
    bool is_empty_line( const string & line );

    /**
     * Split 'line' into a content and a comment part that are returned in
     * 'content_ret' and 'comment_ret', respectively. 'comment_ret' is either
     * empty, or it starts with the comment marker. 'content_ret' is stripped
     * of trailing whitespace.
     **/
    void split_off_comment( const string & line,
			    string & content_ret,
			    string & comment_ret );

    /**
     * Strip all trailing whitespace from 'line'.
     **/
    void strip_trailing_whitespace( string & line );

    /**
     * Return the line number of the end of the header comment or -1 if there
     * is none.
     **/
    int find_header_comment_end( const string_vec & lines );

    /**
     * Return the line number of the start of the footer comment (starting with
     * line number 'from' or -1 if there is none.
     **/
    int find_footer_comment_start( const string_vec & lines, int from );

    /**
     * Parse entries from line no. 'from' to line no. 'end' in 'lines'.
     * Return 'true' if success, 'false' if error.
     **/
    bool parse_entries( const string_vec & lines, int from, int end );


private:

    // By default, file permissions are only limited by umask value
    static const int DEFAULT_PERMISSIONS = 0666;

    string	    filename;
    int 	    permissions;
    string comment_marker = "#";
    bool inline_comments = true;
    bool diff_enabled = false;

    string_vec	    header_comments;
    vector<Entry *> entries;
    string_vec	    footer_comments;
    string_vec      orig_lines;

};

} // namespace storage

#endif // CommentedConfigFile_h
