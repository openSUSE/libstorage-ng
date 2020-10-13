/*
 * Copyright (c) 2018 SUSE LLC
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


#include "storage/SimpleEtcFstab.h"
#include "storage/EtcFstab.h"


namespace storage
{

    vector<SimpleEtcFstabEntry>
    read_simple_etc_fstab(const string& filename)
    {
	EtcFstab etc_fstab(filename);

	vector<SimpleEtcFstabEntry> ret;

	for (int i = 0; i < etc_fstab.get_entry_count(); ++i)
	{
	    const FstabEntry* fstab_entry = etc_fstab.get_entry(i);

	    SimpleEtcFstabEntry tmp;
	    
	    tmp.device = fstab_entry->get_spec();
	    tmp.mount_point = fstab_entry->get_mount_point();
	    tmp.fs_type = fstab_entry->get_fs_type();
	    tmp.mount_options = fstab_entry->get_mount_opts().get_opts();
	    tmp.fs_freq = fstab_entry->get_dump_pass();
	    tmp.fs_passno = fstab_entry->get_fsck_pass();

	    ret.push_back(tmp);
	}

	return ret;
    }

}
