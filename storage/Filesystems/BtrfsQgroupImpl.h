/*
 * Copyright (c) [2020-2021] SUSE LLC
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


#ifndef STORAGE_BTRFS_QGROUP_IMPL_H
#define STORAGE_BTRFS_QGROUP_IMPL_H


#include "storage/Filesystems/BtrfsQgroup.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Actions/SetLimitsImpl.h"


namespace storage
{

    using namespace std;


    class BtrfsSubvolume;


    template <> struct DeviceTraits<BtrfsQgroup> { static const char* classname; };


    class BtrfsQgroup::Impl : public Device::Impl
    {

    public:

	static const id_t unknown_id;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual void add_dependencies(Actiongraph::Impl& actiongraph) const override;

    public:

	Impl(const id_t& id);

	Impl(const xmlNode* node);

	virtual bool is_in_view(View view) const override;

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual const char* get_classname() const override { return DeviceTraits<BtrfsQgroup>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	Btrfs* get_btrfs();
	const Btrfs* get_btrfs() const;

	bool has_btrfs_subvolume() const;
	const BtrfsSubvolume* get_btrfs_subvolume() const;

	id_t get_id() const { return id; }
	void set_id(const id_t& id) { Impl::id = id; }

	unsigned long long get_referenced() const { return referenced; }
	void set_referenced(unsigned long long referenced) { Impl::referenced = referenced; }

	unsigned long long get_exclusive() const { return exclusive; }
	void set_exclusive(unsigned long long exclusive) { Impl::exclusive = exclusive; }

	std::optional<unsigned long long> get_referenced_limit() const { return referenced_limit; }
	void set_referenced_limit(const std::optional<unsigned long long>& referenced_limit)
	    { Impl::referenced_limit = referenced_limit; }

	std::optional<unsigned long long> get_exclusive_limit() const { return exclusive_limit; }
	void set_exclusive_limit(const std::optional<unsigned long long>& exclusive_limit)
	    { Impl::exclusive_limit = exclusive_limit; }

	bool is_assigned(const BtrfsQgroup* btrfs_qgroup) const;

	void assign(BtrfsQgroup* btrfs_qgroup);
	void unassign(BtrfsQgroup* btrfs_qgroup);

	vector<BtrfsQgroup*> get_assigned_btrfs_qgroups();
	vector<const BtrfsQgroup*> get_assigned_btrfs_qgroups() const;

	virtual BtrfsQgroup* get_non_impl() override { return to_btrfs_qgroup(Device::Impl::get_non_impl()); }
	virtual const BtrfsQgroup* get_non_impl() const override { return to_btrfs_qgroup(Device::Impl::get_non_impl()); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_set_limits_text(const CommitData& commit_data, const Action::SetLimits* action) const;
	virtual void do_set_limits(CommitData& commit_data, const Action::SetLimits* action);
	virtual uf_t do_set_limits_used_features() const { return UF_BTRFS; }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

	static id_t parse_id(const string& str);

	static string format_id(const id_t& id);

    private:

	id_t id = unknown_id;

	unsigned long long referenced = 0;
	unsigned long long exclusive = 0;

	std::optional<unsigned long long> referenced_limit;
	std::optional<unsigned long long> exclusive_limit;

    };

}

#endif
