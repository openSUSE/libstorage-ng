/*
 * Copyright (c) [2016-2018] SUSE LLC
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


#ifndef STORAGE_PROBER_H
#define STORAGE_PROBER_H


#include <string>
#include <vector>
#include <functional>


namespace storage
{
    using std::string;
    using std::vector;


    class ProbeCallbacks;
    class Devicegraph;
    class SystemInfo;
    class Device;


    struct SysBlockEntries
    {
	vector<string> disks;
	vector<string> dasds;
	vector<string> stray_blk_devices;
	vector<string> mds;
	vector<string> bcaches;
    };


    /**
     * Read entries in /sys/block and based on the name and further
     * information place the name (excluding starting /dev) one of the vectors
     * in SysBlockEntries.
     *
     * Note: It is important that additional check as to whether create a Node
     * in the devicegraph are done in probe_sys_block_entries since the result
     * is also used for other functions (e.g. light_probe).
     */
    SysBlockEntries probe_sys_block_entries(SystemInfo& system_info);


    /**
     * Class for probing.
     */
    class Prober
    {
    public:

	/**
	 * The constructor probes the system and places the result in system.
	 */
	Prober(const ProbeCallbacks* probe_callbacks, Devicegraph* system, SystemInfo& system_info);

	const ProbeCallbacks* get_probe_callbacks() const { return probe_callbacks; }

	Devicegraph* get_system() { return system; }

	SystemInfo& get_system_info() { return system_info; }

	typedef std::function<void(Devicegraph* system, Device* a, Device* b)> add_holder_func_t;

	/**
	 * Adds a holder by calling add_holder_func. If the BlkDevice 'name'
	 * is not found add_holder_func is called later in
	 * flush_pending_holders().
	 */
	void add_holder(const string& name, Device* b, add_holder_func_t add_holder_func);

	const SysBlockEntries& get_sys_block_entries() const { return sys_block_entries; }

    private:

	const ProbeCallbacks* probe_callbacks;

	Devicegraph* system;

	SystemInfo& system_info;

	SysBlockEntries sys_block_entries;

	struct pending_holder_t
	{
	    pending_holder_t(const string& name, Device* b, add_holder_func_t add_holder_func)
		: name(name), b(b), add_holder_func(add_holder_func) {}

	    string name;
	    Device* b;
	    add_holder_func_t add_holder_func;
	};

	vector<pending_holder_t> pending_holders;

	/**
	 * Flushes the pendings holders. If a BlkDevice is still not found an
	 * exception is thrown.
	 */
	void flush_pending_holders();

    };

}


#endif
