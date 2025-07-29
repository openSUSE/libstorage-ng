/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#include <cstdlib>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "storage/EtcFstab.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"


#define FSTAB_COLUMN_COUNT	6


namespace storage
{

    MountOpts::MountOpts( const string & opt_string )
    {
        if ( ! opt_string.empty() )
            parse( opt_string );
    }


    MountOpts::MountOpts(const vector<string> & opts)
	: opts(opts)
    {
	remove("defaults");
    }


    string MountOpts::get_opt( int index ) const
    {
	if ( index >= 0 && index < (int) opts.size() )
	    return opts[ index ];
	else
	    return "";
    }


    void MountOpts::set_opt( int index, const string & new_val )
    {
	if ( index >= 0 && index < (int) opts.size() )
	    opts[ index ] = new_val;
    }


    bool MountOpts::contains( const string & wanted_opt ) const
    {
	for ( size_t i=0; i < opts.size(); i++)
	{
	    if ( opts[i] == wanted_opt )
		return true;
	}

	return false;
    }


    bool
    MountOpts::has_subvol() const
    {
	return any_of(opts.begin(), opts.end(), [](const string& opt) {
	    return boost::starts_with(opt, "subvolid=") || boost::starts_with(opt, "subvol=");
	});
    }


    bool
    MountOpts::has_subvol(long id) const
    {
	regex re_id("subvolid=0*" + to_string(id), regex::extended);

	return any_of(opts.begin(), opts.end(), [re_id](const string& opt) {
	    return regex_match(opt, re_id);
	});
    }


    bool
    MountOpts::has_subvol(long id, const string& path) const
    {
	regex re_id("subvolid=0*" + to_string(id), regex::extended);
	regex re_path("subvol=/*" + regex_escape(path, regex::extended), regex::extended);

	return any_of(opts.begin(), opts.end(), [re_id, re_path](const string& opt) {
	    return regex_match(opt, re_id) || regex_match(opt, re_path);
	});
    }


    int MountOpts::get_index_of( const string & wanted_opt ) const
    {
	for ( size_t i=0; i < opts.size(); i++)
	{
	    if ( opts[i] == wanted_opt )
		return i;
	}

	return -1;
    }


    void MountOpts::remove( int index )
    {
	if ( index >= 0 && index < (int) opts.size() )
	    opts.erase( opts.begin() + index );
    }


    void MountOpts::remove( const string & opt )
    {
	opts.erase(std::remove(opts.begin(), opts.end(), opt), opts.end());
    }


    void MountOpts::append( const string & opt )
    {
	opts.push_back( opt );
    }


    string MountOpts::format() const
    {
	if ( opts.empty() )
	    return "defaults";

	string result;

	for ( size_t i=0; i < opts.size(); ++i )
	{
	    if ( ! result.empty() )
		result += ",";

	    result += opts[i];
	}

	return result;
    }


    bool MountOpts::parse( const string & opt_string, int line_no )
    {
        string decoded = EtcFstab::decode(opt_string);
	boost::split( opts,
		      decoded,
		      boost::is_any_of( "," ),
		      boost::token_compress_on );

	remove("defaults");

        return true;
    }


    FstabEntry::FstabEntry(const string& spec, const string& mount_point, FsType fs_type)
	: spec(spec), fs_type(fs_type)
    {
        set_mount_point(mount_point);
    }


    FstabEntry::~FstabEntry()
    {
    }


    void FstabEntry::set_mount_point( const string & new_val )
    {
        mount_point = MountPoint::normalize_path(new_val);
    }


    bool FstabEntry::validate()
    {
        bool ok = true;

        if (spec.empty())
        {
            ok = false;
            y2err("No spec specified for entry " << get_mount_point());
        }

        if (get_mount_point().empty())
        {
            ok = false;
            y2err("No mount point specified for entry " << spec);
        }

        return ok;
    }


    void FstabEntry::populate_columns()
    {
	set_column_count( FSTAB_COLUMN_COUNT );

	int col = 0;
	set_column(col++, EtcFstab::encode(spec));
	set_column(col++, EtcFstab::encode(get_mount_point()));

        if ( fs_type != FsType::UNKNOWN )
            set_column( col++, toString( fs_type ) );
        else
        {
            if ( ! get_column( col ).empty() )
                col++; // just leave the old content
            else
            {
                y2err( "File system type unknown for " << spec << " at " << get_mount_point() );
                set_column( col++, "unknown" );
            }
        }

	set_column( col++, mount_opts.format() );
	set_column( col++, std::to_string( dump_pass ) );
	set_column( col++, std::to_string( fsck_pass ) );
    }


    bool FstabEntry::parse( const string & line, int line_no )
    {
	// Delegate splitting into fields to parent class.
	//
	// This always returns 'true' (success), so there is no need to check
	// the result and possibly log an error.
	//
	// Note that the last two fstab columns (fs_freq and fs_passno) are optional with default to 0.

	ColumnConfigFile::Entry::parse( line, line_no );

	int column_count = get_column_count();

	if ( column_count < FSTAB_COLUMN_COUNT - 2 || column_count > FSTAB_COLUMN_COUNT )
	{
	    y2err( "fstab:" << line_no << " Error: wrong number of fields: \"" << line << "\"" );
	    return false;
	}

	int col = 0;
	spec = EtcFstab::decode(get_column(col++));
	set_mount_point(EtcFstab::decode(get_column(col++)));

	if (!toValue( get_column( col++ ), fs_type ))
            fs_type = FsType::UNKNOWN;

	mount_opts.parse(EtcFstab::decode(get_column(col++)), line_no);

	dump_pass = 0;
	fsck_pass = 0;

	if ( column_count >= FSTAB_COLUMN_COUNT - 1 )
	    dump_pass = atoi( get_column( col++ ).c_str() );

	if ( column_count == FSTAB_COLUMN_COUNT )
	    fsck_pass = atoi( get_column( col++ ).c_str() );

	return true; // success
    }


    MountByType
    FstabEntry::get_mount_by() const
    {
	return EtcFstab::get_mount_by(spec);
    }



    EtcFstab::EtcFstab(const string& filename) :
	ColumnConfigFile()
    {
	// Set reasonable field widths for /etc/fstab

	int col = 0;
	set_max_column_width( col++, 45 ); // spec; just enough for UUID=...
	set_max_column_width( col++, 25 ); // mount point
	set_max_column_width( col++, 10 ); // fs type
	set_max_column_width( col++, 30 ); // mount options
	set_max_column_width( col++,  1 ); // dump pass
	set_max_column_width( col++,  1 ); // fsck pass

	set_pad_columns( true );
        set_diff_enabled();

	if (!filename.empty())
	    read(filename);
    }


    EtcFstab::~EtcFstab()
    {
    }


    vector<FstabEntry*>
    EtcFstab::find_all_by_spec_and_mount_point(const string& spec, const string& mount_point)
    {
	vector<FstabEntry*> ret;

	for (int i = 0; i < get_entry_count(); ++i)
	{
	    FstabEntry* entry = get_entry(i);
	    if (spec == entry->get_spec() && mount_point == entry->get_mount_point())
		ret.push_back(entry);
	}

	return ret;
    }


    FstabEntry * EtcFstab::find_mount_point( const string & mount_point, int & index_ret ) const
    {
	for ( int i=0; i < get_entry_count(); ++i )
	{
	    FstabEntry * entry = get_entry( i );

	    if ( entry && entry->get_mount_point() == mount_point )
	    {
		index_ret = i;
		return entry;
	    }
	}

	index_ret = -1;
	return 0;
    }


    MountByType
    EtcFstab::get_mount_by(const string& spec)
    {
	if (boost::starts_with(spec, "UUID=") ||
	    boost::starts_with(spec, DEV_DISK_BY_UUID_DIR "/"))
	    return MountByType::UUID;

	if (boost::starts_with(spec, "LABEL=") ||
	    boost::starts_with(spec, DEV_DISK_BY_LABEL_DIR "/"))
	    return MountByType::LABEL;

	if (boost::starts_with(spec, DEV_DISK_BY_ID_DIR "/"))
	    return MountByType::ID;

	if (boost::starts_with(spec, DEV_DISK_BY_PATH_DIR "/"))
	    return MountByType::PATH;

	if (boost::starts_with(spec, "PARTUUID=") ||
	    boost::starts_with(spec, DEV_DISK_BY_PARTUUID_DIR "/"))
	    return MountByType::PARTUUID;

	if (boost::starts_with(spec, "PARTLABEL=") ||
	    boost::starts_with(spec, DEV_DISK_BY_PARTLABEL_DIR "/"))
	    return MountByType::PARTLABEL;

	return MountByType::DEVICE;
    }


    FstabEntry * EtcFstab::get_entry( int index ) const
    {
        ST_CHECK_INDEX( index, 0, get_entry_count() - 1 );

        CommentedConfigFile::Entry * entry =
            CommentedConfigFile::get_entry( index );

        ST_CHECK_PTR( entry );

        return dynamic_cast<FstabEntry *>( entry );
    }


    void EtcFstab::add( FstabEntry * entry )
    {
        int index = find_sort_index( entry );

        if ( index < 0 )
            append( entry );
        else
            insert( index, entry );
    }


    int EtcFstab::find_sort_index( FstabEntry * entry ) const
    {
        ST_CHECK_PTR( entry );

        string mount_point = entry->get_mount_point();

        for ( int i=0; i < get_entry_count(); ++i )
        {
            // Insert 'entry' before anything that is mounted into its mount tree

            if ( boost::starts_with( get_entry(i)->get_mount_point(), mount_point ) &&
                 entry != get_entry(i) )
            {
                return i;
            }
        }

        return -1;
    }


    int EtcFstab::next_mount_order_problem( int start_index ) const
    {
        for ( int i=start_index; i < get_entry_count(); ++i )
        {
            int index = find_sort_index( get_entry(i) );

            if ( index != -1 && index < i )
                return i;
        }

        return -1;
    }


    bool EtcFstab::check_mount_order() const
    {
        int index = next_mount_order_problem();

        return index == -1;
    }


    void EtcFstab::fix_mount_order()
    {
        int start_index = 0;

        while ( start_index < get_entry_count() )
        {
            int problem_index = next_mount_order_problem( start_index );

            if ( problem_index == -1 )
                return;

            // Take this entry out of the entries vector and put it back in at
            // the correct place.

            FstabEntry * entry = dynamic_cast<FstabEntry *>( take( problem_index ) );
            add( entry );

            // By definition, the entries vector is now fixed up to this index,
            // so continue fixing at the next index.

            start_index = problem_index + 1;

            // There is one pathological case, though:
            //
            // When two or more entries have the same mount point (which is
            // illegal, but somebody might write such an fstab manually),
            // there is no correct mount order; this algorithm would get into
            // an endless loop if we now checked the same index again. But by
            // just proceeding with the next one (and silently assuming that
            // the one we just changed is well and truly fixed), we can avoid
            // that endless loop.
        }
    }


    string
    EtcFstab::encode(const string& unencoded)
    {
	string tmp = unencoded;

	// see fstab(5) and getmntent(3)

	boost::replace_all(tmp, "\\", "\\\\");

	boost::replace_all(tmp, "\t", "\\011");
	boost::replace_all(tmp, "\n", "\\012");
	boost::replace_all(tmp, " ", "\\040");

	return tmp;
    }


    string
    EtcFstab::decode(const string& encoded)
    {
	string tmp = encoded;

	// see fstab(5) and getmntent(3)

	boost::replace_all(tmp, "\\011", "\t");
	boost::replace_all(tmp, "\\012", "\n");
	boost::replace_all(tmp, "\\040", " ");
	boost::replace_all(tmp, "\\123", "\\");

	boost::replace_all(tmp, "\\\\", "\\");

	return tmp;
    }


    void EtcFstab::log()
    {
        string_vec lines = format_lines();

        if ( ! get_filename().empty() )
            y2mil( get_filename() );

        for ( size_t i=0; i < lines.size(); ++i )
            y2mil( lines[i] );
    }


    void EtcFstab::log_diff()
    {
        string_vec lines = diff();

        if ( ! get_filename().empty() )
            y2mil( get_filename() << " diff: " );

        for ( size_t i=0; i < lines.size(); ++i )
            y2mil( lines[i] );
    }


    vector<string>
    JointEntry::get_mount_options() const
    {
	if (fstab_entry)
	    return fstab_entry->get_mount_opts().get_opts();

	if (mount_entry)
	    return mount_entry->get_mount_opts().get_opts();

	ST_THROW(Exception("neither fstab nor mount entry set"));
    }


    MountByType
    JointEntry::get_mount_by() const
    {
	if (fstab_entry)
	    return fstab_entry->get_mount_by();

	return MountByType::DEVICE;
    }


    FsType
    JointEntry::get_fs_type() const
    {
	if (fstab_entry)
	    return fstab_entry->get_fs_type();

	if (mount_entry)
	    return mount_entry->get_fs_type();

	return FsType::UNKNOWN;
    }


    MountPoint*
    JointEntry::add_to(Mountable* mountable) const
    {
	MountPoint* mount_point = mountable->create_mount_point(get_mount_point());
	mount_point->set_rootprefixed(is_rootprefixed());

	if (is_in_etc_fstab())
	    mount_point->get_impl().set_fstab_anchor(FstabAnchor(fstab_entry->get_spec(), id,
								 fstab_entry->get_mount_point()));

	mount_point->set_mount_by(get_mount_by());

	// In some cases the fs_type is unknown, e.g. a NTFS included in
	// /proc/mounts (as fuseblk) but not in /etc/fstab.
	if (get_fs_type() != FsType::UNKNOWN)
	    mount_point->set_mount_type(get_fs_type());

	mount_point->set_mount_options(get_mount_options());

	mount_point->set_in_etc_fstab(is_in_etc_fstab());
	mount_point->set_active(is_active());

	return mount_point;
    }


    vector<JointEntry>
    join_entries(vector<ExtendedFstabEntry> fstab_entries, vector<ExtendedFstabEntry> mount_entries)
    {
	vector<JointEntry> joint_entries;

	for (const ExtendedFstabEntry& extended_fstab_entry : fstab_entries)
	{
	    joint_entries.push_back(
		JointEntry(extended_fstab_entry.mount_point_path, extended_fstab_entry.fstab_entry, nullptr,
			   extended_fstab_entry.id));
	}

	for (const ExtendedFstabEntry& extended_fstab_entry : mount_entries)
	{
	    const MountPointPath& mount_point_path = extended_fstab_entry.mount_point_path;

	    vector<JointEntry>::iterator it = find_if(joint_entries.begin(), joint_entries.end(),
		[&mount_point_path, &extended_fstab_entry](const JointEntry& joint_entry) {
		    if (!joint_entry.is_in_etc_fstab())
			return false;

		    if (joint_entry.fstab_entry->get_fs_type() == FsType::SWAP &&
			extended_fstab_entry.fstab_entry->get_fs_type() == FsType::SWAP)
			return true;

		    return joint_entry.mount_point_path == mount_point_path;
		}
	    );

	    if (it != joint_entries.end())
	    {
		it->mount_entry = extended_fstab_entry.fstab_entry;
	    }
	    else
	    {
		y2war("mount point for " << extended_fstab_entry.fstab_entry->get_spec() <<
		      " only found in proc/mounts: " << extended_fstab_entry.fstab_entry->get_mount_point());

		joint_entries.push_back(JointEntry(extended_fstab_entry.mount_point_path, nullptr,
						   extended_fstab_entry.fstab_entry));
	    }
	}

	return joint_entries;
    }


    bool
    compare_by_size(const JointEntry* a, const JointEntry* b)
    {
	return a->get_mount_point().size() < b->get_mount_point().size();
    }

}
