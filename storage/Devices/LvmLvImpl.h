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


#ifndef STORAGE_LVM_LV_IMPL_H
#define STORAGE_LVM_LV_IMPL_H


#include "storage/Devices/LvmLv.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/Enum.h"
#include "storage/Actions/RenameImpl.h"


namespace storage
{

    using namespace std;

    class ActivateCallbacks;


    template <> struct DeviceTraits<LvmLv> { static const char* classname; };

    template <> struct EnumTraits<LvType> { static const vector<string> names; };


    class LvmLv::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& vg_name, const string& lv_name, LvType lv_type);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<LvmLv>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return get_lv_name(); }

	static bool activate_lvm_lvs(const ActivateCallbacks* activate_callbacks);

	static bool deactivate_lvm_lvs();

	static void probe_lvm_lvs(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual bool is_usable_as_blk_device() const override;

	const LvmVg* get_lvm_vg() const;

	const LvmLv* get_thin_pool() const;

	bool has_snapshots() const;

	vector<LvmLv*> get_snapshots();
	vector<const LvmLv*> get_snapshots() const;

	bool has_origin() const;

	LvmLv* get_origin();
	const LvmLv* get_origin() const;

	unsigned long long max_size_for_lvm_lv(LvType lv_type, const vector<sid_t>& ignore_sids = {}) const;

	LvmLv* create_lvm_lv(const string& lv_name, LvType lv_type, unsigned long long size);

	const string& get_lv_name() const { return lv_name; }
	void set_lv_name(const string& lv_name);

	static bool is_valid_lv_name(const string& lv_name);

	LvType get_lv_type() const { return lv_type; }

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	virtual void set_region(const Region& region) override;

	unsigned long long get_used_extents() const { return used_extents; }
	void set_used_extents(unsigned long long used_extents) { Impl::used_extents = used_extents; }

	bool supports_stripes() const;

	unsigned int get_stripes() const { return stripes; }
	void set_stripes(unsigned int stripes);

	unsigned long long get_stripe_size() const { return stripe_size; }
	void set_stripe_size(unsigned long long stripe_size);

	bool supports_chunk_size() const;

	unsigned long long get_chunk_size() const { return chunk_size; }
	void set_chunk_size(unsigned long long chunk_size);

	static unsigned long long default_chunk_size(unsigned long long size);

	unsigned long long default_chunk_size() const;

	static unsigned long long default_metadata_size(unsigned long long size,
							unsigned long long chunk_size,
							unsigned long long extent_size);

	unsigned long long default_metadata_size() const;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	LvmLv* get_lvm_lv(const string& lv_name);

	vector<LvmLv*> get_lvm_lvs();
	vector<const LvmLv*> get_lvm_lvs() const;

	static LvmLv* find_by_uuid(Devicegraph* devicegraph, const string& uuid);
	static const LvmLv* find_by_uuid(const Devicegraph* devicegraph, const string& uuid);

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	virtual LvmLv* get_non_impl() override { return to_lvm_lv(Device::Impl::get_non_impl()); }
	virtual const LvmLv* get_non_impl() const override { return to_lvm_lv(Device::Impl::get_non_impl()); }

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;
	virtual void do_create_post_verify() const override;
	virtual uf_t do_create_used_features() const override { return UF_LVM; }

	virtual Text do_rename_text(const CommitData& commit_data, const Action::Rename* action) const;
	virtual void do_rename(const CommitData& commit_data, const Action::Rename* action) const;
	virtual uf_t do_rename_used_features() const { return UF_LVM; }

	virtual Text do_resize_text(const CommitData& commit_data, const Action::Resize* action) const override;
	virtual void do_resize(const CommitData& commit_data, const Action::Resize* action) const override;
	virtual uf_t do_resize_used_features() const override { return UF_LVM; }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_LVM; }

	virtual Text do_activate_text(Tense tense) const override;
	virtual void do_activate() override;
	virtual uf_t do_activate_used_features() const override { return UF_LVM; }

	virtual Text do_deactivate_text(Tense tense) const override;
	virtual void do_deactivate() override;
	virtual uf_t do_deactivate_used_features() const override { return UF_LVM; }

    private:

	static string make_name(const string& vg_name, const string& lv_name);

	static string make_dm_table_name(const string& vg_name, const string& lv_name);

	string lv_name;
	LvType lv_type;
	string uuid;

	/**
	 * Number of extent used in the volume group.
	 *
	 * For thins the value is zero since the extents are already used by the thin
	 * pool.
	 *
	 * For thick snapshots this is different from the size/region. The size/region
	 * gives the size of the block device while the used extents gives the space
	 * allocated for the differences to the origin. Note: The block device size of the
	 * snapshot may be different from the block device size of the origin (which may
	 * have been resized since the snapshot was made).
	 *
	 * TODO For RAIDs and mirrors the value is incorrect.
	 */
	unsigned long long used_extents = 0;

	unsigned int stripes = 0;
	unsigned long long stripe_size = 0;

	unsigned long long chunk_size = 0;

    };

}

#endif
