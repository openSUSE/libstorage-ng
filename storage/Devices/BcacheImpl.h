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


#ifndef STORAGE_BCACHE_IMPL_H
#define STORAGE_BCACHE_IMPL_H


#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/PartitionableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Bcache> { static const char* classname; };


    class Bcache::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Bcache"; }

	virtual string get_pretty_classname() const override;

	virtual string get_sort_key() const override;

	static bool is_valid_name(const string& name);

	static void probe_bcaches(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;
	virtual void probe_pass_1b(Prober& prober) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual ResizeInfo detect_resize_info() const override;

	virtual void save(xmlNode* node) const override;

	virtual uint64_t used_features() const override;

	unsigned int get_number() const;

	const BlkDevice* get_blk_device() const;

	bool has_bcache_cset() const;

	const BcacheCset* get_bcache_cset() const;

	void attach_bcache_cset(BcacheCset* bcache_cset);

	void update_sysfs_name_and_path();

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void parent_has_new_region(const Device* parent) override;

	static void run_dependency_manager(Actiongraph::Impl& actiongraph);

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

	virtual Text do_deactivate_text(Tense tense) const override;
	virtual void do_deactivate() const override;

	Text do_attach_bcache_cset_text(Tense tense) const;
	void do_attach_bcache_cset() const;

    private:

	static const unsigned long long metadata_size = 8 * KiB;

	void calculate_region();

    };


    namespace Action
    {

	class AttachBcacheCset : public Modify
	{
	public:

	    AttachBcacheCset(sid_t sid)
		: Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	};

    }

}

#endif
