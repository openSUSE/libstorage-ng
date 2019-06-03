/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2017-2019] SUSE LLC
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
#include "storage/SystemInfo/SystemInfo.h"


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
	while (contains("defaults"))
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
	regex re_path("subvol=/*" + path, regex::extended);

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
	int index = get_index_of( opt );

	if ( index != -1 )
	    remove( index );
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
        string decoded = EtcFstab::fstab_decode( opt_string );
	boost::split( opts,
		      decoded,
		      boost::is_any_of( "," ),
		      boost::token_compress_on );

	while ( contains( "defaults" ) )
	    remove( "defaults" );

        return true;
    }




    FstabEntry::FstabEntry()
	: fs_type(FsType::UNKNOWN), dump_pass(0), fsck_pass(0)
    {
    }


    FstabEntry::FstabEntry( const string & device,
			    const string & mount_point,
			    FsType	   fs_type ):
	device( device ),
	fs_type( fs_type ),
	dump_pass( 0 ),
	fsck_pass( 0 )
    {
        set_mount_point(mount_point);
    }


    FstabEntry::~FstabEntry()
    {

    }

    void FstabEntry::set_mount_point( const string & new_val )
    {
        _mount_point = MountPoint::normalize_path(new_val);
    }

    bool FstabEntry::validate()
    {
        bool ok = true;

        if ( device.empty() )
        {
            ok = false;
            y2err( "No device specified for entry " << get_mount_point() );
        }

        if ( get_mount_point().empty() )
        {
            ok = false;
            y2err( "No mount point specified for entry " << device );
        }

        return ok;
    }


    void FstabEntry::populate_columns()
    {
	set_column_count( FSTAB_COLUMN_COUNT );

	int col = 0;
	set_column( col++, EtcFstab::fstab_encode( device      ) );
	set_column( col++, EtcFstab::fstab_encode( get_mount_point() ) );

        if ( fs_type != FsType::UNKNOWN )
            set_column( col++, toString( fs_type ) );
        else
        {
            if ( ! get_column( col ).empty() )
                col++; // just leave the old content
            else
            {
                y2err( "File system type unknown for " << device << " at " << get_mount_point() );
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

	ColumnConfigFile::Entry::parse( line, line_no );

	if ( get_column_count() != FSTAB_COLUMN_COUNT )
	{
	    y2err( "fstab:" << line_no << " Error: wrong number of fields: \"" << line << "\"" );
	    return false;
	}

	int col = 0;
	device	    = EtcFstab::fstab_decode( get_column( col++ ) );
	set_mount_point( EtcFstab::fstab_decode( get_column( col++ ) ) );

	bool ok = toValue( get_column( col++ ), fs_type );

	if ( ! ok )
            fs_type = FsType::UNKNOWN;

	mount_opts.parse( EtcFstab::fstab_decode( get_column( col++ ) ), line_no );
	dump_pass = atoi( get_column( col++ ).c_str() );
	fsck_pass = atoi( get_column( col++ ).c_str() );

	return true; // success
    }


    MountByType
    FstabEntry::get_mount_by() const
    {
	return EtcFstab::get_mount_by(device);
    }



    EtcFstab::EtcFstab(const string& filename) :
	ColumnConfigFile()
    {
	// Set reasonable field widths for /etc/fstab

	int col = 0;
	set_max_column_width( col++, 45 ); // device; just enough for UUID=...
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


    FstabEntry * EtcFstab::find_device( const string & device  ) const
    {
	for ( int i=0; i < get_entry_count(); ++i )
	{
	    FstabEntry * entry = get_entry( i );

	    if ( entry && entry->get_device() == device )
		return entry;
	}

	return 0;
    }


    vector<FstabEntry*>
    EtcFstab::find_all_devices(const string& device)
    {
	vector<FstabEntry*> ret;

	for (int i = 0; i < get_entry_count(); ++i)
	{
	    FstabEntry* entry = get_entry(i);
	    if (device == entry->get_device())
		ret.push_back(entry);
	}

	return ret;
    }


    vector<const FstabEntry*>
    EtcFstab::find_all_by_uuid_or_label(const string& uuid, const string& label) const
    {
	vector<const FstabEntry*> ret;

	for (int i = 0; i < get_entry_count(); ++i)
	{
	    const FstabEntry* entry = get_entry(i);

	    string blk_device = entry->get_device();

	    if (!uuid.empty())
	    {
		if ((blk_device == "UUID=" + uuid) ||
		    (blk_device == DEV_DISK_BY_UUID_DIR "/" + uuid))
		    ret.push_back(entry);
	    }

	    if (!label.empty())
	    {
		if ((blk_device == "LABEL=" + label) ||
		    (blk_device == DEV_DISK_BY_LABEL_DIR "/" + udev_encode(label)))
		    ret.push_back(entry);
	    }
	}

	return ret;
    }


    vector<const FstabEntry*>
    EtcFstab::find_all_by_any_name(SystemInfo& system_info, const string& name) const
    {
	dev_t majorminor = system_info.getCmdUdevadmInfo(name).get_majorminor();

	vector<const FstabEntry*> ret;

	for (int i = 0; i < get_entry_count(); ++i)
	{
	    const FstabEntry* entry = get_entry(i);

	    string blk_device = entry->get_device();

	    if (boost::starts_with(blk_device, DEV_DIR "/"))
	    {
		try
		{
		    if (system_info.getCmdUdevadmInfo(blk_device).get_majorminor() == majorminor)
			ret.push_back(entry);
		}
		catch (const Exception& exception)
		{
		    // The block device for the fstab entry may not be available right
		    // now so the exception is not necessarily an error. Likely the noauto
		    // option is present but even that is not required.
		    ST_CAUGHT(exception);
		}
	    }
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


    MountByType EtcFstab::get_mount_by( const string & device )
    {
	if ( boost::starts_with( device, "UUID=" ) ||
	     boost::starts_with( device, DEV_DISK_BY_UUID_DIR "/" ) )
	{
	    return MountByType::UUID;
	}

	if ( boost::starts_with( device, "LABEL=" ) ||
	     boost::starts_with( device, DEV_DISK_BY_LABEL_DIR "/" ) )
	{
	    return MountByType::LABEL;
	}

	if ( boost::starts_with( device, DEV_DISK_BY_ID_DIR "/" ) )
	    return MountByType::ID;

	if ( boost::starts_with( device, DEV_DISK_BY_PATH_DIR "/" ) )
	    return MountByType::PATH;

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
            // illegal, but somebody might write such an fstab manuallly),
            // there is no correct mount order; this algorithm would get into
            // an endless loop if we now checked the same index again. But by
            // just proceeding with the next one (and silently assuming that
            // the one we just changed is well and truly fixed), we can avoid
            // that endless loop.
        }
    }


    string EtcFstab::fstab_encode( const string & unencoded )
    {
	return boost::replace_all_copy( unencoded, " ", "\\040" );
    }


    string EtcFstab::fstab_decode( const string & encoded )
    {
	return boost::replace_all_copy( encoded, "\\040", " " );
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


    string
    JointEntry::get_mount_point() const
    {
	if (mount_entry)
	    return mount_entry->get_mount_point();

	if (fstab_entry)
	    return fstab_entry->get_mount_point();

	ST_THROW(Exception("neither fstab nor mount entry set"));
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

	if (is_in_etc_fstab())
	    mount_point->get_impl().set_fstab_anchor(FstabAnchor(fstab_entry->get_device(), id));

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
	vector<JointEntry> ret;

	for (const ExtendedFstabEntry& extended_fstab_entry : fstab_entries)
	{
	    ret.push_back(JointEntry(extended_fstab_entry.fstab_entry, nullptr, extended_fstab_entry.id));
	}

	for (const ExtendedFstabEntry& extended_fstab_entry : mount_entries)
	{
	    string path = extended_fstab_entry.fstab_entry->get_mount_point();

	    // for multiple mount points for same partition try at first identical path
	    // and if not found, then any
	    vector<JointEntry>::iterator it = find_if(ret.begin(), ret.end(), [&path](const JointEntry& tmp) {
		if (!tmp.is_in_etc_fstab())
		    return false;

		return tmp.fstab_entry->get_mount_point() == path;
	    });

	    if (it == ret.end())
	    {
		it = find_if(ret.begin(), ret.end(), [&path](const JointEntry& tmp) {
		    bool in_fstab = tmp.is_in_etc_fstab();
		    if (in_fstab && tmp.fstab_entry->get_mount_point() != path)
		    {
			y2war("mount points for " << tmp.fstab_entry->get_device()
			      << " differ: fstab(" << tmp.fstab_entry->get_mount_point()
			      << ") != proc(" << path << ")");
		    }
		    return in_fstab;
		});
	    };

	    if (it != ret.end())
		it->mount_entry = extended_fstab_entry.fstab_entry;
	    else
		ret.push_back(JointEntry(nullptr, extended_fstab_entry.fstab_entry));
	}

	return ret;
    }


    bool
    compare_by_size(const JointEntry& a, const JointEntry& b)
    {
	return a.get_mount_point().size() < b.get_mount_point().size();
    }

}
