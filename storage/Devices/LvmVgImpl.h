/*
 * Copyright (c) [2016-2025] SUSE LLC
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


#ifndef STORAGE_LVM_VG_IMPL_H
#define STORAGE_LVM_VG_IMPL_H


#include "storage/Utils/HumanString.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Actions/RenameImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<LvmVg> { static const char* classname; };


    class LvmVg::Impl : public Device::Impl
    {
    public:

	static const unsigned long long default_extent_size = 4 * MiB;

	static const unsigned long long max_extent_number = std::numeric_limits<uint32_t>::max();

	Impl(const string& vg_name);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<LvmVg>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return get_vg_name(); }

	virtual string get_name_sort_key() const override { return DEV_DIR "/" + vg_name; }

	static void probe_lvm_vgs(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;

	void calculate_reserved_extents(Prober& prober);

	unsigned long long get_reserved_extents() const { return reserved_extents; }
	void set_reserved_extents(unsigned long long reserved_extents) { Impl::reserved_extents = reserved_extents; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	const Region& get_region() const { return region; }

	unsigned long long get_size() const;

	Text get_size_text() const;

	unsigned long long get_extent_size() const { return region.get_block_size(ULL_HACK); }
	void set_extent_size(unsigned long long extent_size);

	virtual void parent_has_new_region(const Device* parent) override;

	unsigned long long number_of_extents() const { return region.get_length(); }
	unsigned long long number_of_used_extents(const vector<sid_t>& ignore_sids = {}) const;
	unsigned long long number_of_free_extents(const vector<sid_t>& ignore_sids = {}) const;

	bool is_overcommitted() const;

	const string& get_vg_name() const { return vg_name; }
	void set_vg_name(const string& vg_name);

	static bool is_valid_vg_name(const string& vg_name);

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	LvmPv* add_lvm_pv(BlkDevice* blk_device);
	void remove_lvm_pv(BlkDevice* blk_device);

	vector<LvmPv*> get_lvm_pvs();
	vector<const LvmPv*> get_lvm_pvs() const;

	unsigned long long max_size_for_lvm_lv(LvType lv_type, const vector<sid_t>& ignore_sids = {}) const;

	LvmLv* create_lvm_lv(const string& lv_name, LvType lv_type, unsigned long long size);
	void delete_lvm_lv(LvmLv* lvm_lv);

	LvmLv* get_lvm_lv(const string& lv_name);

	vector<LvmLv*> get_lvm_lvs();
	vector<const LvmLv*> get_lvm_lvs() const;

	static LvmVg* find_by_uuid(Devicegraph* devicegraph, const string& uuid);
	static const LvmVg* find_by_uuid(const Devicegraph* devicegraph, const string& uuid);

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	/**
	 * Check whether the volume group is partial (one or more physical
	 * volumes belonging to the volume group are missing from the system
	 * (see vgs(8)).
	 */
	bool is_partial() const;

	static void run_dependency_manager(Actiongraph::Impl& actiongraph);

	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;
	virtual void do_create_post_verify() const override;
	virtual uf_t do_create_used_features() const override { return UF_LVM; }

	virtual Text do_rename_text(const CommitData& commit_data, const Action::Rename* action) const;
	virtual void do_rename(const CommitData& commit_data, const Action::Rename* action) const;
	virtual uf_t do_rename_used_features() const { return UF_LVM; }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_LVM; }

	virtual Text do_reallot_text(const CommitData& commit_data, const Action::Reallot* action) const override;
	virtual void do_reallot(const CommitData& commit_data, const Action::Reallot* action) const override;
	virtual uf_t do_reallot_used_features() const override { return UF_LVM; }

	void do_reduce(const LvmPv* lvm_pv) const;
	void do_extend(const LvmPv* lvm_pv) const;

	Text do_reduce_missing_text(Tense tense) const;
	void do_reduce_missing() const;

	virtual void add_dependencies(Actiongraph::Impl& actiongraph) const override;

    private:

	string vg_name;
	string uuid;

	/**
	 * Holds extent-size and extent-count.
	 */
	Region region;

	/**
	 * Number of reserved extents in the volume group. Those reserved
	 * extents are not used by libstorage-ng. Also see doc/lvm.md.
	 */
	unsigned long long reserved_extents = 0;

	void calculate_region();

	/**
	 * Checks if the given action is realloting this volume group.
	 */
	bool action_is_my_reallot(const Action::Base* action, const Actiongraph::Impl& actiongraph) const;

	/**
	 * Checks if the given action is resizing a physical volume of this volume group.
	 */
	bool action_is_my_pv_resize(const Action::Base* action, const Actiongraph::Impl& actiongraph) const;

	/**
	 * Checks if the action performed in a logical volume decreases the free
	 * space available in the volume group.
	 */
	bool action_uses_vg_space(const Action::Base* action, const Actiongraph::Impl& actiongraph) const;

	/**
	 * Checks if the action performed in a logical volume increases the free
	 * space available in the volume group.
	 */
	bool action_frees_vg_space(const Action::Base* action, const Actiongraph::Impl& actiongraph) const;

	/**
	 * Checks whether the device is a logical volume of the volume group.
	 */
	bool is_my_lvm_lv(const Device* device) const;

	/**
	 * Checks whether the device is a logical volume of the volume group
	 * using extents (so excluding thin logical volumes).
	 */
	bool is_my_lvm_lv_using_extents(const Device* device) const;

    };

}

#endif
