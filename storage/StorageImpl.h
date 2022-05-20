/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2022] SUSE LLC
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


#ifndef STORAGE_STORAGE_IMPL_H
#define STORAGE_STORAGE_IMPL_H


#include "storage/Devices/Device.h"
#include "storage/Utils/FileUtils.h"
#include "storage/Utils/LockImpl.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/Arch.h"
#include "storage/CommitOptions.h"


namespace storage
{
    using std::string;
    using std::map;


    class LuksInfo::Impl
    {
    public:

	string device_name;
	unsigned long long size = 0;
	string dm_table_name;
	bool is_dm_table_name_generated = false;
	string uuid;
	string label;

    };


    class BitlockerInfo::Impl
    {
    public:

	string device_name;
	unsigned long long size = 0;
	string dm_table_name;
	bool is_dm_table_name_generated = false;
	string uuid;

    };


    class DeactivateStatusV2::Impl
    {
    public:

	bool multipath = false;
	bool dm_raid = false;
	bool md = false;
	bool lvm_lv = false;
	bool luks = false;
	bool bitlocker = false;

    };


    class Storage::Impl
    {
    public:

	Impl(Storage& storage, const Environment& environment);
	~Impl();

    public:

	const Environment& get_environment() const { return environment; }

	Arch& get_arch() { return arch; }
	const Arch& get_arch() const { return arch; }

	Devicegraph* create_devicegraph(const string& name);
	Devicegraph* copy_devicegraph(const string& source_name, const string& dest_name);
	void remove_devicegraph(const string& name);
	void restore_devicegraph(const string& name);

	bool equal_devicegraph(const string& lhs, const string& rhs) const;

	bool exist_devicegraph(const string& name) const;

	vector<string> get_devicegraph_names() const;
	map<string, const Devicegraph*> get_devicegraphs() const;

	Devicegraph* get_devicegraph(const string& name);
	const Devicegraph* get_devicegraph(const string& name) const;

	Devicegraph* get_staging();
	const Devicegraph* get_staging() const;

	const Devicegraph* get_probed() const;

	Devicegraph* get_system();
	const Devicegraph* get_system() const;

	void check(const CheckCallbacks* check_callbacks) const;

	MountByType get_default_mount_by() const { return default_mount_by; }
	void set_default_mount_by(MountByType default_mount_by) { Impl::default_mount_by = default_mount_by; }

	const string& get_rootprefix() const { return rootprefix; }
	void set_rootprefix(const string& rootprefix) { Impl::rootprefix = rootprefix; }

	string prepend_rootprefix(const string& mount_point) const;

	const Actiongraph* calculate_actiongraph();

	void activate(const ActivateCallbacks* activate_callbacks) const;

	DeactivateStatusV2 deactivate() const;

	void probe(const ProbeCallbacks* probe_callbacks);

	void commit(const CommitOptions& commit_options, const CommitCallbacks* commit_callbacks);

	void generate_pools(const Devicegraph* devicegraph);

	Pool* create_pool(const string& name);

	void remove_pool(const string& name);

	void rename_pool(const string& old_name, const string& new_name);

	bool exists_pool(const string& name) const;

	vector<string> get_pool_names() const;

	map<string, Pool*> get_pools();
	map<string, const Pool*> get_pools() const;

	Pool* get_pool(const string& name);
	const Pool* get_pool(const string& name) const;

	const TmpDir& get_tmp_dir() const { return tmp_dir; }

	/**
	 * Get the next sid.
	 */
	static sid_t get_next_sid() { return global_sid++; }

	/**
	 * Raises the global sid to avoid potential conflicts with sid.
	 */
	static void raise_global_sid(sid_t sid) { global_sid = max(global_sid, sid + 1); }

	/**
	 * Resets the global sid. Only for testsuites.
	 */
	static void reset_global_sid() { global_sid = initial_global_sid; }

    private:

	void verify_devicegraph_name(const string& name) const;
	void verify_pool_name(const string& name) const;

	static const sid_t initial_global_sid = 42;	// just a random number ;)

	static sid_t global_sid;

	void probe_helper(const ProbeCallbacks* probe_callbacks, Devicegraph* system);

	Storage& storage;

	const Environment environment;

	Arch arch;

	Lock lock;

	using devicegraphs_t = map<string, Devicegraph>;
	devicegraphs_t devicegraphs;

	using pools_t = map<string, Pool>;
	pools_t pools;

	MountByType default_mount_by;

	string rootprefix;

	std::unique_ptr<const Actiongraph> actiongraph;

	TmpDir tmp_dir;

    };

}

#endif
