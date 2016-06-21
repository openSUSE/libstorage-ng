/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_STORAGE_IMPL_H
#define STORAGE_STORAGE_IMPL_H


#include <map>

#include "storage/Utils/FileUtils.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/Arch.h"


namespace storage
{
    using std::string;
    using std::map;


    class Storage::Impl
    {
    public:

	Impl(const Storage& storage, const Environment& environment);
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
	std::vector<std::string> get_devicegraph_names() const;

	Devicegraph* get_devicegraph(const string& name);
	const Devicegraph* get_devicegraph(const string& name) const;

	Devicegraph* get_staging();
	const Devicegraph* get_staging() const;

	const Devicegraph* get_probed() const;

	void check() const;

	const string& get_rootprefix() const { return rootprefix; }
	void set_rootprefix(const string& rootprefix) { Impl::rootprefix = rootprefix; }

	string prepend_rootprefix(const string& mountpoint) const;

	const Actiongraph* calculate_actiongraph();

	void commit(const CommitCallbacks* commit_callbacks);

	const TmpDir& get_tmp_dir() const { return tmp_dir; }

    private:

	void probe(Devicegraph* probed);

	const Storage& storage;

	const Environment environment;

	Arch arch;

	map<string, Devicegraph> devicegraphs;

	string rootprefix;

	std::unique_ptr<const Actiongraph> actiongraph;

	TmpDir tmp_dir;

    };

}

#endif
