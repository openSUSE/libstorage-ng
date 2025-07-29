/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2017-2025] SUSE LLC
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


#ifndef STORAGE_ETC_FSTAB_H
#define STORAGE_ETC_FSTAB_H


#include <string>
#include <vector>

#include "storage/Utils/ColumnConfigFile.h"
#include "storage/Utils/MountPointPath.h"
#include "storage/Filesystems/Filesystem.h"
#include "storage/SystemInfo/SystemInfo.h"

#define ETC_FSTAB "/etc/fstab"


namespace storage
{
    using std::string;
    using std::vector;

    class BlkDevice;
    class BlkFilesystem;


    /**
     * Helper class for the mount options of one /etc/fstab entry.
     * In /etc/fstab, the mount options are a comma-separated list.
     * This class keeps them as separate options.
     *
     * "defaults" is not stored in this class; this class reports itself as
     * empty in that case.
     **/
    class MountOpts
    {
    public:
	/**
	 * Constructor. Parse 'opt_string' if non-empty, create empty mount
	 * options otherwise.
	 **/
	MountOpts( const string & opt_string = "" );

	/**
	 * Constructor. Sets options to 'opts'.
	 */
	MountOpts(const vector<string>& opts);

	/**
	 * Return the number of mount options.
	 **/
	size_t size() const { return opts.size(); }

	/**
	 * Return 'true' if the mount options are empty, i.e. the corresponding
	 * field in the /etc/fstab entry would be "defaults".
	 **/
	bool empty() const { return opts.empty(); }

	/**
	 * Check whether two MountOpts objects are equal.
	 */
	bool operator==(const MountOpts& rhs) const { return opts == rhs.opts; }

	/**
	 * Check whether two MountOpts objects are unequal.
	 */
	bool operator!=(const MountOpts& rhs) const { return !(*this == rhs); }

	/**
	 * Return a const_iterator referring to the first mount option.
	 **/
	string_vec::const_iterator begin() const { return opts.begin(); }

	/**
	 * Return a const_iterator referring one element behind the last mount
	 * option.
	 **/
	string_vec::const_iterator end()   const { return opts.end();	}

	/**
	 * Return the mount option with the specified index or an empty string
	 * if 'index' is out of bounds.
	 **/
	string get_opt( int index ) const;

	/**
	 * Set the mount option with the specified index to a new value.
	 **/
	void set_opt( int index, const string & new_val );

	/**
	 * Return 'true' if any of the mount options is exactly the content of
	 * 'opt'. Notice that "defaults" is not stored here; use empty()
	 * instead.
	 **/
	bool contains( const string & opt ) const;

	/**
	 * Return true iff the options include 'subvol=' or 'subvolid='.
	 */
	bool has_subvol() const;

	/**
	 * Return true iff the options include 'subvol=id'. Fuzzy concerning
	 * leading zeros for ids.
	 */
	bool has_subvol(long id) const;

	/**
	 * Return true iff the options include 'subvol=id' or 'subvolid=path'.
	 * Fuzzy concerning leading zeros for ids and leading slashes for paths.
	 */
	bool has_subvol(long id, const string& path) const;

	/**
	 * Find the position of a mount option: Return the index of the mount
	 * option matching exactly 'opt' or -1 if there is no match.
	 **/
	int get_index_of( const string & opt ) const;

	/**
	 * Remove a mount option by index (starting with 0).
	 **/
	void remove( int index );

	/**
	 * Remove a mount option that matches exactly the contents of 'opt'.
	 **/
	void remove( const string & opt );

	/**
	 * Add a mount option to the end of the existing ones.
	 * See also operator<<().
	 **/
	void append( const string & opt );

        /**
         * Clear all mount options.
         **/
        void clear() { opts.clear(); }

	/**
	 * Very much like append(), but also return a reference to this object
	 * so options can be chained:
	 *
	 *     opts << "noauto" << "user";
	 **/
	MountOpts & operator<<( const string & opt ) { append( opt ); return *this; }

	/**
	 * Format the contents for use in /etc/fstab; if there are no mount
	 * options, return "defaults".
	 **/
	string format() const;

	/**
	 * Parse mount options from the fourth field of an /etc/fstab entry.
	 * "defaults" is treated as empty options (i.e. for the purpose of this
	 * class, "defaults" is simply ignored).
	 *
	 * This returns 'true' on success, 'false' on error.
	 **/
	bool parse( const string & opt_string, int line_no = -1 );

        /**
         * Return the options as string vector. If there are no options, this
         * is empty; it will not return "defaults" in that case.
         **/
        const string_vec & get_opts() const { return opts; }

        /**
         * Set the options as string vector. If there are no options, this
         * should be empty, not contain a string "defaults".
         **/
        void set_opts( const string_vec & new_opts ) { opts = new_opts; }

    protected:

	string_vec opts;
    };


    /**
     * Class representing one /etc/fstab entry.
     **/
    class FstabEntry : public ColumnConfigFile::Entry
    {
    public:

	FstabEntry() = default;

	FstabEntry(const string & spec, const string& mount_point, FsType fs_type);

	virtual ~FstabEntry();

        /**
         * Validate this entry just prior to formatting/writing it back to
         * file: Return 'true' if okay, 'false' if not.
         *
         * If this returns 'false', the entry is not formatted/written -
         * neither the entry itself nor the comments that belong to it. Error
         * handling such as logging an error or whatever is this function's
         * responsibility.
         *
	 * Reimplemented from CommentedConfigFile.
         **/
        virtual bool validate() override;

	/**
	 * Populate the columns with content from the member variables.
	 *
	 * Reimplemented from ColumnConfigFile.
	 **/
	virtual void populate_columns() override;

	/**
	 * Parse a content line. Return 'true' on success, 'false' on error.
	 *
	 * Reimplemented from CommentedConfigFile.
	 **/
	virtual bool parse(const string& line, int line_no = -1) override;


	// Getters; see man fstab(5)

	const string &	  get_spec()	    const { return spec;	}
	const string &	  get_mount_point() const { return mount_point; }
	FsType		  get_fs_type()	    const { return fs_type;	}
	const MountOpts & get_mount_opts()  const { return mount_opts;	}
	int		  get_dump_pass()   const { return dump_pass;	}
	int		  get_fsck_pass()   const { return fsck_pass;	}

	// Setters

	void set_spec	    ( const string &	new_val ) { spec	= new_val; }
	void set_mount_point( const string &	new_val );
	void set_fs_type    ( FsType		new_val ) { fs_type	= new_val; }
	void set_mount_opts ( const MountOpts & new_val ) { mount_opts	= new_val; }
	void set_dump_pass  ( int		new_val ) { dump_pass	= new_val; }
	void set_fsck_pass  ( int		new_val ) { fsck_pass	= new_val; }

	/**
	 * Convenience function calling EtcFstab::get_mount_by(get_spec()).
	 */
	MountByType get_mount_by() const;

    private:

	string	  spec;		// including UUID=, LABEL=, PARTUUID=, ...
	string	  mount_point;	// always use set_mount_point()
	FsType	  fs_type = FsType::UNKNOWN;	// see Filesystems/Filesystem.h
	MountOpts mount_opts;
	int	  dump_pass = 0;	// historic
	int	  fsck_pass = 0;
    };


    /**
     * Class representing /etc/fstab.
     **/
    class EtcFstab : public ColumnConfigFile
    {
    public:

	EtcFstab(const string& filename = ETC_FSTAB);
	virtual ~EtcFstab();

	// using inherited read() and write() unchanged

	/**
	 * Find all entries where the 'spec' and 'mount_point' match.
	 */
	vector<FstabEntry*> find_all_by_spec_and_mount_point(const string& spec, const string& mount_point);

	/**
	 * Return the first entry for mount point 'mount_point' or 0 if there
	 * is no matching entry.
	 **/
	FstabEntry * find_mount_point( const string & mount_point ) const
	    { int dummy; return find_mount_point( mount_point, dummy ); }

	/**
	 * Return the first entry for mount point 'mount_point' or 0 if there
	 * is no matching entry. 'index_ret' returns its index in the entries
	 * or -1 if there was no match.
	 **/
	FstabEntry * find_mount_point( const string & mount_point, int & index_ret ) const;

        /**
         * Add an entry at the correct place to maintain the mount order.
         **/
        void add( FstabEntry * entry );

        /**
         * Check the mount order in the current entries. Return 'true' if ok,
         * 'false' if not.
         **/
        bool check_mount_order() const;

        /**
         * Fix the mount order in the current entries.
         **/
        void fix_mount_order();

        /**
         * Return entry no. 'index'. Unlike the inherited method, this throws
	 * an IndexOutOfRangeException if the index is out of range.
         *
         * This is a covariant of the (non-virtual) base class method to reduce
         * the number of dynamic_casts.
         **/
        FstabEntry * get_entry( int index ) const;

        /**
         * Factory method to create one entry.
         *
         * Reimplemented from CommentedConfigFile.
         **/
        virtual Entry* create_entry() override { return new FstabEntry(); }


	/**
	 * Get the "mount by" type from a device specification. This might include:
	 *
	 *   - "UUID="		      => UUID
	 *   - "LABEL="		      => LABEL
	 *   - "/dev/disk/by-uuid/"   => UUID
	 *   - "/dev/disk/by-label/"  => LABEL
	 *   - "/dev/disk/by-id/"     => ID
	 *   - "/dev/disk/by-path/"   => PATH
	 *
	 * Everything else is DEVICE.
	 **/
	static MountByType get_mount_by(const string& spec);

	/**
	 * Encode a string that might contain whitespace for use in /etc/fstab:
	 * Use the corresponding octal sequence instead (\040 for blank etc.).
	 **/
	static string encode(const string& unencoded);

	/**
	 * Decode an fstab-encoded string: Change back \040 to blank etc.
	 **/
	static string decode(const string& encoded);

        /**
         * Dump the current contents to the log.
         **/
        void log();

        /**
         * Dump a diff to the previous state before the last read() / parse() /
         * write().
         **/
        void log_diff();

    protected:

        /**
         * Find the entry index before which 'entry' should be inserted into
         * the sort order or -1 if it should become the last entry
         * (i.e. appended).
         **/
        int find_sort_index( FstabEntry * entry ) const;

        /**
         * Return the entry index with the next problem in the mount order or
         * -1 if there is none (i.e. everything is ok). Start checking from
         * 'start_index' onwards.
         **/
        int next_mount_order_problem( int start_index = 0 ) const;


        // Change privacy of some inherited methods.
        //
        // Use add() instead which takes care of the correct insertion order.

        void insert( int before, CommentedConfigFile::Entry * entry )
            { CommentedConfigFile::insert( before, entry ); }

        void append( CommentedConfigFile::Entry * entry )
            { CommentedConfigFile::append( entry ); }

        CommentedConfigFile & operator<<( CommentedConfigFile::Entry * entry )
            { return CommentedConfigFile::operator<<( entry ); }
    };


    /**
     * An extended fstab entry that contains a MountPointPath, an FstabEntry pointer and
     * FilesystemUser.id.
     */
    struct ExtendedFstabEntry
    {
	ExtendedFstabEntry(const MountPointPath& mount_point_path, const FstabEntry* fstab_entry,
			   unsigned int id = 0)
	    : mount_point_path(mount_point_path), fstab_entry(fstab_entry), id(id) {}

	const MountPointPath mount_point_path;
	const FstabEntry* fstab_entry;
	const unsigned int id;
    };


    /**
     * Joint entry of entries in /etc/fstab and /proc/mounts.
     */
    struct JointEntry
    {
	JointEntry(const MountPointPath& mount_point_path, const FstabEntry* fstab_entry,
		   const FstabEntry* mount_entry, unsigned int id = 0)
	    : mount_point_path(mount_point_path), fstab_entry(fstab_entry), mount_entry(mount_entry), id(id) {}

	const string& get_mount_point() const { return mount_point_path.path; }
	bool is_rootprefixed() const { return mount_point_path.rootprefixed; }

	vector<string> get_mount_options() const;
	MountByType get_mount_by() const;

	bool is_in_etc_fstab() const { return fstab_entry; }
	bool is_active() const { return mount_entry; }

	FsType get_fs_type() const;

	MountPoint* add_to(Mountable* mountable) const;

	const MountPointPath mount_point_path;
	const FstabEntry* fstab_entry;
	const FstabEntry* mount_entry;
	const unsigned int id;
    };


    /**
     * Joins entries from /etc/fstab and /proc/mounts that have the same mount point.
     */
    vector<JointEntry>
    join_entries(vector<ExtendedFstabEntry> fstab_entries, vector<ExtendedFstabEntry> mount_entries);


    /**
     * Compare join entries by its mount point length.
     */
    bool compare_by_size(const JointEntry* a, const JointEntry* b);

}


#endif
