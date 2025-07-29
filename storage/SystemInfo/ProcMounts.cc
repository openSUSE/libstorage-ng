/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{
    using namespace std;


    ProcMounts::ProcMounts()
    {
	AsciiFile mounts(PROC_DIR "/mounts");
	AsciiFile swaps(PROC_DIR "/swaps");

	parse_proc_mounts_lines(mounts.get_lines());
	parse_proc_swaps_lines(swaps.get_lines());

	y2mil(*this);
    }


    ProcMounts::~ProcMounts()
    {
        clear();
    }


    void
    ProcMounts::clear()
    {
	for (value_type& value : data)
            delete value.second;

        data.clear();
    }


    void
    ProcMounts::parse_proc_mounts_lines(const vector<string>& lines)
    {
        EtcFstab fstab("");
        fstab.parse( lines );

        while ( ! fstab.empty())
        {
            FstabEntry * entry = dynamic_cast<FstabEntry *>( fstab.take(0) );

	    if (entry)
	    {
		const string& spec = entry->get_spec();
		FsType fs_type = entry->get_fs_type();

		if (boost::starts_with(spec, "/dev/") && spec != "/dev/root")
		{
		    data.emplace(spec, entry);
		}
		else if (fs_type == FsType::NFS || fs_type == FsType::NFS4 ||
			 fs_type == FsType::TMPFS)
		{
		    data.emplace(spec, entry);
		}
		else
		{
		    // Get rid of all the useless stuff that clutters /proc/mounts
		    delete entry;
		}
	    }
        }
    }


    void
    ProcMounts::parse_proc_swaps_lines(const vector<string>& lines)
    {
	for (size_t i = 1; i < lines.size(); ++i)
	{
	    string spec = EtcFstab::decode(extractNthWord(0, lines[i]));

	    string::size_type pos = spec.find(" (deleted)");
	    if (pos != string::npos)
		spec.erase(pos);

	    FstabEntry* entry = new FstabEntry(spec, "swap", FsType::SWAP);
	    data.emplace(spec, entry);
	}
    }


    std::ostream&
    operator<<(std::ostream& s, const ProcMounts& procmounts)
    {
	for (ProcMounts::const_iterator it = procmounts.data.begin(); it != procmounts.data.end(); ++it)
        {
            FstabEntry * entry = it->second;
            entry->populate_columns();
	    s << "data[" << it->first << "] -> " << entry->format() << '\n';
        }

	return s;
    }


    vector<const FstabEntry*>
    ProcMounts::get_by_name(const string& name, SystemInfo::Impl& system_info) const
    {
	vector<const FstabEntry*> ret;

	// TODO: Lookup with major and minor number only for names of block
	// devices (starting with '/dev/'). Parameter name will also be
	// e.g. 'tmpfs' and nfs mounts.

	dev_t majorminor = system_info.getCmdUdevadmInfo(name).get_majorminor();

	for (const value_type& value : data)
	{
	    if (value.first == name ||
		(BlkDevice::Impl::is_valid_name(value.first) &&
		 system_info.getCmdUdevadmInfo(value.first).get_majorminor() == majorminor))
		ret.push_back(value.second);
	}

	return ret;
    }


    vector<const FstabEntry*>
    ProcMounts::get_all_nfs() const
    {
	vector<const FstabEntry*> ret;

	for (const value_type& value : data)
	{
	    if (value.second->get_fs_type() == FsType::NFS ||
                value.second->get_fs_type() == FsType::NFS4)
            {
		ret.push_back(value.second);
            }
	}

	return ret;
    }


    vector<const FstabEntry*>
    ProcMounts::get_all_tmpfs() const
    {
	vector<const FstabEntry*> ret;

	for (const value_type& value : data)
	{
	    if (value.second->get_fs_type() == FsType::TMPFS)
	    {
		ret.push_back(value.second);
	    }
	}

	return ret;
    }

}
