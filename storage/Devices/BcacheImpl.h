/*
 * Copyright (c) [2016-2019] SUSE LLC
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


#ifndef STORAGE_BCACHE_IMPL_H
#define STORAGE_BCACHE_IMPL_H


#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devicegraph.h"
#include "storage/ActiongraphImpl.h"
#include "storage/Prober.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    template <> struct EnumTraits<BcacheType> { static const vector<string> names; };

    template <> struct DeviceTraits<Bcache> { static const char* classname; };

    class Bcache::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name);

	Impl(const xmlNode* node);

	virtual BcacheType get_type() const = 0;

	virtual const char* get_classname() const override { return "Bcache"; }

	virtual string get_pretty_classname() const override;

	virtual string get_sort_key() const override;

	static bool is_valid_name(const string& name);

	static void probe_bcaches(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;

	virtual ResizeInfo detect_resize_info() const override;

	virtual uint64_t used_features() const override;

	unsigned int get_number() const;
	void set_number(unsigned int number);

	const BcacheCset* get_bcache_cset() const;

	static string find_free_name(const Devicegraph* devicegraph);

	static void reassign_numbers(Devicegraph* devicegraph);

	void update_sysfs_name_and_path();

	static void run_dependency_manager(Actiongraph::Impl& actiongraph);

    };

}

#endif
