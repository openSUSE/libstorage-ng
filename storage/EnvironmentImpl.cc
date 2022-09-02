/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2018-2022] SUSE LLC
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


#include <langinfo.h>
#include <string.h>
#include <ostream>

#include "storage/EnvironmentImpl.h"


namespace storage
{

    Environment::Impl::Impl(bool read_only, ProbeMode probe_mode, TargetMode target_mode)
	: read_only(read_only), probe_mode(probe_mode), target_mode(target_mode)
    {
	const char* p1 = getenv("LIBSTORAGE_ROOTPREFIX");
	if (p1)
	    rootprefix = p1;

	const char* p2 = getenv("LIBSTORAGE_LOCKFILE_ROOT");
	if (p2)
	    lockfile_root = p2;
    }


    bool
    Environment::Impl::is_do_lock() const
    {
	switch (probe_mode)
	{
	    case ProbeMode::STANDARD:
	    case ProbeMode::STANDARD_WRITE_DEVICEGRAPH:
	    case ProbeMode::STANDARD_WRITE_MOCKUP:
		return true;

	    case ProbeMode::NONE:
	    case ProbeMode::READ_DEVICEGRAPH:
	    case ProbeMode::READ_MOCKUP:
		return false;
	}

	return true;
    }


    std::ostream&
    operator<<(std::ostream& out, const Environment::Impl& environment)
    {
	out << "read-only:" << environment.read_only << " probe-mode:"
	    << toString(environment.probe_mode) << " target-mode:"
	    << toString(environment.target_mode);

	if (!environment.rootprefix.empty())
	    out << " rootprefix:" << environment.rootprefix;

	if (!environment.lockfile_root.empty())
	    out << " lockfile-root:" << environment.lockfile_root;

	return out;
    }


    const vector<string> EnumTraits<ProbeMode>::names({
	"STANDARD", "STANDARD_WRITE_DEVICEGRAPH", "STANDARD_WRITE_MOCKUP", "NONE",
	"READ_DEVICEGRAPH", "READ_MOCKUP"
    });


    const vector<string> EnumTraits<TargetMode>::names({
	"DIRECT", "CHROOT", "IMAGE"
    });


    static bool
    read_env_var(const char* name, bool fallback)
    {
	const char* p = getenv(name);
	return p ? strcmp(p, "yes") == 0 : fallback;
    }


    bool
    support_btrfs_multiple_devices()
    {
	return read_env_var("LIBSTORAGE_MULTIPLE_DEVICES_BTRFS", true);
    }


    bool
    support_btrfs_snapshot_relations()
    {
	return read_env_var("LIBSTORAGE_BTRFS_SNAPSHOT_RELATIONS", true);
    }


    bool
    support_btrfs_qgroups()
    {
	return read_env_var("LIBSTORAGE_BTRFS_QGROUPS", true);
    }


    bool
    developer_mode()
    {
	return read_env_var("LIBSTORAGE_DEVELOPER_MODE", false);
    }


    bool
    prefer_filesystem_over_empty_msdos()
    {
	return read_env_var("LIBSTORAGE_PFSOEMS", true);
    }


    bool
    cryptsetup_for_bitlocker()
    {
	return read_env_var("LIBSTORAGE_CRYPTSETUP_FOR_BITLOCKER", false);
    }


    int
    mdadm_activate_method()
    {
	const char* p = getenv("LIBSTORAGE_MDADM_ACTIVATE_METHOD");
	return p ? atoi(p) : 0;
    }


    void
    Environment::Impl::extra_log()
    {
	y2mil("codeset " << nl_langinfo(CODESET));

	const char* env_vars[] = {
	    "PATH",
	    "LD_LIBRARY_PATH",
	    "LD_PRELOAD",
	    "LIBSTORAGE_BTRFS_QGROUPS",
	    "LIBSTORAGE_BTRFS_SNAPSHOT_RELATIONS",
	    "LIBSTORAGE_DEVELOPER_MODE",
	    "LIBSTORAGE_LOCKFILE_ROOT",
	    "LIBSTORAGE_MDADM_ACTIVATE_METHOD",
	    "LIBSTORAGE_MULTIPLE_DEVICES_BTRFS",
	    "LIBSTORAGE_PFSOEMS",
	    "LIBSTORAGE_ROOTPREFIX",
	};

	for (const char* env_var : env_vars)
	{
	    const char* value = getenv(env_var);
	    if (value)
		y2mil(env_var << "=" << value);
	}
    }

}
