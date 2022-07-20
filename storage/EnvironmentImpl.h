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


#ifndef STORAGE_ENVIRONMENT_IMPL_H
#define STORAGE_ENVIRONMENT_IMPL_H


#include "storage/Environment.h"
#include "storage/Utils/Enum.h"


namespace storage
{
    using std::string;


    class Environment::Impl
    {
    public:

	Impl(bool read_only, ProbeMode probe_mode, TargetMode target_mode);

	bool is_read_only() const { return read_only; }

	ProbeMode get_probe_mode() const { return probe_mode; }

	TargetMode get_target_mode() const { return target_mode; }

	const string& get_rootprefix() const { return rootprefix; }
	void set_rootprefix(const string& rootprefix);

	const string& get_lockfile_root() const { return lockfile_root; }
	void set_lockfile_root(const string& lock_dir_root) { Impl::lockfile_root = lockfile_root; }

	const string& get_devicegraph_filename() const { return devicegraph_filename; }
	void set_devicegraph_filename(const string& devicegraph_filename);

	const string& get_arch_filename() const { return arch_filename; }
	void set_arch_filename(const string& arch_filename);

	const string& get_mockup_filename() const { return mockup_filename; }
	void set_mockup_filename(const string& mockup_filename);

	bool is_debug_credentials() const { return false; }

	bool is_do_lock() const;

	friend std::ostream& operator<<(std::ostream& out, const Impl& environment);

	static void extra_log();

    private:

	const bool read_only;
	const ProbeMode probe_mode;
	const TargetMode target_mode;

	string rootprefix;
	string lockfile_root;
	string devicegraph_filename;
	string arch_filename;
	string mockup_filename;

    };


    template <> struct EnumTraits<ProbeMode> { static const vector<string> names; };
    template <> struct EnumTraits<TargetMode> { static const vector<string> names; };


    /**
     * Switch to enable multiple device support for btrfs (during probing).
     */
    bool support_btrfs_multiple_devices();

    /**
     * Switch to enable btrfs snapshot relations (during probing).
     */
    bool support_btrfs_snapshot_relations();

    /**
     * Switch to enable btrfs qgroups (during probing).
     */
    bool support_btrfs_qgroups();

    /**
     * Switch to enable developer mode. What this mode exactly does is surely undefined.
     */
    bool developer_mode();

    /**
     * If there is a file system and an empty MS-DOS partition table on a partitionable
     * prefer the file system.
     */
    bool prefer_filesystem_over_empty_msdos();

    /**
     * Switch to use cryptsetup for BitLocker (during probing) (Bitlocker
     * vs. BitlockerV2).
     */
    bool cryptsetup_for_bitlocker();

}

#endif
