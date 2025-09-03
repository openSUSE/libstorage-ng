/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_MD_IMPL_H
#define STORAGE_MD_IMPL_H


#include <regex>

#include "storage/Devices/Md.h"
#include "storage/Devices/PartitionableImpl.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    class ActivateCallbacks;
    class TmpDir;


    template <> struct DeviceTraits<Md> { static const char* classname; };

    template <> struct EnumTraits<MdLevel> { static const vector<string> names; };
    template <> struct EnumTraits<MdParity> { static const vector<string> names; };


    class Md::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Md>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_name_sort_key() const override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual void set_name(const string& name) override;

	MdUser* add_device(BlkDevice* blk_device);
	void remove_device(BlkDevice* blk_device);

	virtual vector<BlkDevice*> get_blk_devices();
	virtual vector<const BlkDevice*> get_blk_devices() const;

	virtual void parent_has_new_region(const Device* parent) override;

	virtual string pool_name() const override { return ""; }

	bool is_numeric() const;

	unsigned int get_number() const;

	/**
	 * Get the short name or RAID name of the RAID. E.g. "foo" for "/dev/md/foo".
	 */
	string get_short_name() const;

	MdLevel get_md_level() const { return md_level; }
	void set_md_level(MdLevel md_level);

	MdParity get_md_parity() const { return md_parity; }
	void set_md_parity(MdParity md_parity) { Impl::md_parity = md_parity; }

	vector<MdParity> get_allowed_md_parities() const;

	bool is_chunk_size_meaningful() const;

	unsigned long get_chunk_size() const { return chunk_size; }
	void set_chunk_size(unsigned long chunk_size);

	static unsigned long get_default_chunk_size();

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	const string& get_metadata() const { return metadata; }
	void set_metadata(const string& metadata) { Impl::metadata = metadata; }

	static unsigned int minimal_number_of_devices(MdLevel md_level);

	unsigned int minimal_number_of_devices() const;
	bool supports_spare_devices() const;
	bool supports_journal_device() const;

	unsigned int number_of_devices() const;

	bool is_in_etc_mdadm() const { return in_etc_mdadm; }
	void set_in_etc_mdadm(bool in_etc_mdadm) { Impl::in_etc_mdadm = in_etc_mdadm; }

	static bool is_valid_name(const string& name);

	static bool is_valid_sysfs_name(const string& name);

	virtual vector<MountByType> possible_mount_bys() const override;

	static string find_free_numeric_name(const Devicegraph* devicegraph);

	static unsigned long long calculate_underlying_size(MdLevel md_level, unsigned int number_of_devices,
							    unsigned long long size);

	static bool activate_mds(const ActivateCallbacks* activate_callbacks, const TmpDir& tmp_dir);

	static bool deactivate_mds();

	static void probe_mds(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;
	virtual void probe_pass_1b(Prober& prober) override;
	virtual void probe_pass_1f(Prober& prober) override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void process_udev_paths(vector<string>& udev_paths, const UdevFilters& udev_filters) const override;
	virtual void process_udev_ids(vector<string>& udev_ids, const UdevFilters& udev_filters) const override;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;
	virtual void do_create_post_verify() const override;
	virtual uf_t do_create_used_features() const override { return UF_MDRAID; }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_MDRAID; }

	virtual Text do_add_to_etc_mdadm_text(Tense tense) const;
	virtual void do_add_to_etc_mdadm(CommitData& commit_data) const;

	virtual Text do_remove_from_etc_mdadm_text(Tense tense) const;
	virtual void do_remove_from_etc_mdadm(CommitData& commit_data) const;

	virtual Text do_reallot_text(const CommitData& commit_data, const Action::Reallot* action) const override;
	virtual void do_reallot(const CommitData& commit_data, const Action::Reallot* action) const override;
	virtual uf_t do_reallot_used_features() const override { return UF_MDRAID; }

	void do_reduce(const BlkDevice* blk_device) const;
	void do_extend(const BlkDevice* blk_device) const;

	virtual Text do_deactivate_text(Tense tense) const override;
	virtual void do_deactivate() override;
	virtual uf_t do_deactivate_used_features() const override { return UF_MDRAID; }

	virtual void calculate_region_and_topology();

    protected:

	void probe_uuid();

    private:

	// regex to match name of the form /dev/md<number> and /dev/md/<number>
	static const regex numeric_name_regex;

	// regex to match name of the form /dev/md/<name>
	static const regex format1_name_regex;

	// regex to match name of the form /dev/md_<name>
	static const regex format2_name_regex;

	MdLevel md_level = MdLevel::UNKNOWN;

	MdParity md_parity = MdParity::DEFAULT;

	unsigned long chunk_size = 0;

	/**
	 * The UUID.
	 *
	 * Note: Special MD RAID format.
	 */
	string uuid;

	string metadata;

	bool in_etc_mdadm = true;

	void update_sysfs_name_and_path();

    };

}

#endif
