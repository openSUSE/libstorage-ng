/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"


namespace storage
{
    using namespace std;


    ProcMounts::ProcMounts()
    {
	AsciiFile mounts("/proc/mounts");
	AsciiFile swaps("/proc/swaps");

	parse_proc_mounts_lines(mounts.get_lines());
	parse_proc_swaps_lines(swaps.get_lines());
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
        EtcFstab fstab;
        fstab.parse( lines );

        while ( ! fstab.empty())
        {
            FstabEntry * entry = dynamic_cast<FstabEntry *>( fstab.take(0) );

            if ( entry )
            {
                const string & device = entry->get_device();

                if ( entry->get_fs_type() == FsType::UNKNOWN ||
                     device == "rootfs" ||
                     device == "/dev/root" )
                {
                    // Get rid of all the useless stuff that clutters /proc/mounts
                    delete entry;
                }
                else
                {
                    data.insert( make_pair( device, entry ) );
                }
            }
        }
    }


    void
    ProcMounts::parse_proc_swaps_lines(const vector<string>& lines)
    {
	for (size_t i=1; i < lines.size(); ++i)
	{
	    string dev = EtcFstab::fstab_decode(extractNthWord(0, lines[i]));
	    string::size_type pos = dev.find(" (deleted)");
	    if (pos != string::npos)
		dev.erase(pos);

            FstabEntry * entry = new FstabEntry(dev, "swap", FsType::SWAP);
	    data.insert(make_pair(dev, entry));
	}

	y2mil(*this);
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


    vector<string>
    ProcMounts::find_by_name(const string& name, SystemInfo& systeminfo) const
    {
	vector<string> ret;

	// TODO: Lookup with major and minor number only for names of block
	// devices (starting with '/dev/'). Parameter name will also be
	// e.g. 'tmpfs' and nfs mounts.

	dev_t majorminor = systeminfo.getCmdUdevadmInfo(name).get_majorminor();

	for (const value_type& value : data)
	{
	    if (value.first == name ||
		(BlkDevice::Impl::is_valid_name(value.first) &&
		 systeminfo.getCmdUdevadmInfo(value.first).get_majorminor() == majorminor))
		ret.push_back(value.second->get_mount_point());
	}

	return ret;
    }


    vector<FstabEntry *>
    ProcMounts::get_all_nfs() const
    {
	vector<FstabEntry *> ret;

	for (const value_type& value : data)
	{
	    if (value.second->get_fs_type() == FsType::NFS ||
                value.second->get_fs_type() == FsType::NFS4  )
            {
		ret.push_back(value.second);
            }
	}

	return ret;
    }

}
