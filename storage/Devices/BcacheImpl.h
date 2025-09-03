/*
 * Copyright (c) [2016-2023] SUSE LLC
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
#include "storage/Utils/HumanString.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Bcache> { static const char* classname; };

    template <> struct EnumTraits<BcacheType> { static const vector<string> names; };

    template <> struct EnumTraits<CacheMode> { static const vector<string> names; };


    class Bcache::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name, BcacheType type);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Bcache>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_name_sort_key() const override;

	static bool is_valid_name(const string& name);

	static void probe_bcaches(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;
	virtual void probe_pass_1b(Prober& prober) override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual void save(xmlNode* node) const override;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	unsigned int get_number() const;
	void set_number(unsigned int number);

	const BlkDevice* get_backing_device() const;

	bool has_bcache_cset() const;

	const BcacheCset* get_bcache_cset() const;

	static string find_free_name(const Devicegraph* devicegraph);

	static void reassign_numbers(Devicegraph* devicegraph);

	void attach_bcache_cset(BcacheCset* bcache_cset);

	void detach_bcache_cset();

	void update_sysfs_name_and_path();

	BcacheType get_type() const { return type; }

	virtual string pool_name() const override;

	CacheMode get_cache_mode() const { return cache_mode; }
	void set_cache_mode(CacheMode mode) { cache_mode = mode; }

	unsigned long long get_sequential_cutoff() const { return sequential_cutoff; }
	void set_sequential_cutoff(unsigned long long size) { sequential_cutoff = size; }

	unsigned get_writeback_percent() const { return writeback_percent; }
	void set_writeback_percent(unsigned percent) { writeback_percent = percent; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual Bcache* get_non_impl() override { return to_bcache(Device::Impl::get_non_impl()); }
	virtual const Bcache* get_non_impl() const override { return to_bcache(Device::Impl::get_non_impl()); }

	virtual void parent_has_new_region(const Device* parent) override;

	static void run_dependency_manager(Actiongraph::Impl& actiongraph);

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;
	virtual uf_t do_create_used_features() const override { return UF_BCACHE; }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_BCACHE; }

	virtual Text do_deactivate_text(Tense tense) const override;
	virtual void do_deactivate() override;
	virtual uf_t do_deactivate_used_features() const override { return UF_BCACHE; }

	Text do_attach_bcache_cset_text(Tense tense) const;
	void do_attach_bcache_cset() const;

	Text do_detach_bcache_cset_text(Tense tense, const BcacheCset* bcache_cset) const;
	void do_detach_bcache_cset() const;

	Text do_update_cache_mode_text(Tense tense) const;
	void do_update_cache_mode() const;

	virtual void add_dependencies(Actiongraph::Impl& actiongraph) const override;

    private:

	static const unsigned long long metadata_size = 8 * KiB;

	CacheMode cache_mode;

	unsigned long long sequential_cutoff;

	BcacheType type;

	unsigned writeback_percent;

	void calculate_region();

	/**
	 * Checks if the given action is attaching a bcache cset to this bcache.
	 */
	bool action_is_my_attach(const Action::Base* action, const Actiongraph::Impl& actiongraph) const;

	/**
	 * Checks if the given action is creating the bcache cset that this bcache needs.
	 */
	bool action_is_my_bcache_cset_create(const Action::Base* action, const Actiongraph::Impl& actiongraph) const;

    };

}

#endif
