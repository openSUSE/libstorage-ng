/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_PARTITION_IMPL_H
#define STORAGE_PARTITION_IMPL_H


#include "storage/Utils/Region.h"
#include "storage/Utils/Enum.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    class Partitionable;


    template <> struct DeviceTraits<Partition> { static const char* classname; };

    template <> struct EnumTraits<PartitionType> { static const vector<string> names; };


    class Partition::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name, const Region& region, PartitionType type);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Partition>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_name_sort_key() const override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void probe_pass_1a(Prober& prober) override;

	virtual void probe_topology(Prober& prober) override;

	virtual void save(xmlNode* node) const override;

	virtual bool has_dependency_manager() const override { return true; }

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual bool is_usable_as_blk_device() const override;

	virtual vector<MountByType> possible_mount_bys() const override;

	virtual string get_fstab_spec(MountByType mount_by_type) const override;

	virtual bool spec_match(SystemInfo::Impl& system_info, const string& spec) const override;

	unsigned int get_number() const;
	void set_number(unsigned int number);

	virtual void set_region(const Region& region) override;

	void calculate_topology();

	PartitionType get_type() const { return type; }
	void set_type(PartitionType type);

	unsigned int get_id() const { return id; }
	void set_id(unsigned int id);

	bool is_boot() const { return boot; }
	void set_boot(bool boot);

	bool is_legacy_boot() const { return legacy_boot; }
	void set_legacy_boot(bool legacy_boot);

	bool is_no_automount() const { return no_automount; }
	void set_no_automount(bool no_automount);

	const string& get_label() const { return label; }
	void set_label(const string& label) { Impl::label = label; }

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	void update_name();
	void update_sysfs_name_and_path();
	void update_udev_paths_and_ids();

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual RemoveInfo detect_remove_info() const override;

	Region get_unused_surrounding_region() const;

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

	Partitionable* get_partitionable();
	const Partitionable* get_partitionable() const;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void process_udev_paths(vector<string>& udev_paths, const UdevFilters& udev_filters) const override;
	virtual void process_udev_ids(vector<string>& udev_ids, const UdevFilters& udev_filters) const override;

	virtual Partition* get_non_impl() override { return to_partition(Device::Impl::get_non_impl()); }
	virtual const Partition* get_non_impl() const override { return to_partition(Device::Impl::get_non_impl()); }

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;
	virtual void do_create_post_verify() const override;

	virtual Text do_set_type_id_text(Tense tense) const;
	virtual void do_set_type_id() const;

	virtual Text do_set_label_text(Tense tense) const;
	virtual void do_set_label() const;
	virtual uf_t do_set_label_used_features() const { return 0; }

	virtual Text do_set_boot_text(Tense tense) const;
	virtual void do_set_boot() const;

	virtual Text do_set_legacy_boot_text(Tense tense) const;
	virtual void do_set_legacy_boot() const;

	virtual Text do_set_no_automount_text(Tense tense) const;
	virtual void do_set_no_automount() const;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

	void do_delete_efi_boot_mgr() const;

	virtual Text do_resize_text(const CommitData& commit_data, const Action::Resize* action) const override;
	virtual void do_resize(const CommitData& commit_data, const Action::Resize* action) const override;

	static unsigned int default_id_for_type(PartitionType type);

    private:

	PartitionType type = PartitionType::PRIMARY;
	unsigned int id = ID_LINUX;
	bool boot = false;
	bool legacy_boot = false;
	bool no_automount = false;
	string label;
	string uuid;

	unsigned long long parted_sector_adjustment_factor() const;

	vector<unsigned int> do_create_calc_hack() const;
	void do_create_pre_hack(const vector<unsigned int>& tmps);
	void do_create_post_hack(const vector<unsigned int>& tmps);

	void probe_uuid();

    };


    Text id_to_text(unsigned int id);

}

#endif
