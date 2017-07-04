/*
 * Copyright (c) [2016-2017] SUSE LLC
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


    class Devicegraph;
    class SystemInfo;
    class Device;


    /**
     * Class for probing.
     */
    class Prober
    {
    public:

	/**
	 * The constructor probes the system and places the result in probed.
	 */
	Prober(Devicegraph* probed, SystemInfo& system_info);

	Devicegraph* get_probed() { return probed; }

	SystemInfo& get_system_info() { return system_info; }

	typedef std::function<void(Devicegraph* probed, Device* a, Device* b)> add_holder_func_t;

	/**
	 * Adds a holder by calling add_holder_func. If the BlkDevice 'name'
	 * is not found add_holder_func is called later in
	 * flush_pending_holders().
	 */
	void add_holder(const string& name, Device* b, add_holder_func_t add_holder_func);

    private:

	Devicegraph* probed;

	SystemInfo& system_info;

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
