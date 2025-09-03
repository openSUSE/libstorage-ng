/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_BTRFS_IMPL_H
#define STORAGE_BTRFS_IMPL_H


#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsQgroupImpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Utils/SnapperConfig.h"
#include "storage/Utils/HumanString.h"


namespace storage
{

    using namespace std;


    namespace Action
    {
	class SetQuota;
    }


    template <> struct DeviceTraits<Btrfs> { static const char* classname; };

    template <> struct EnumTraits<BtrfsRaidLevel> { static const vector<string> names; };


    class Btrfs::Impl : public BlkFilesystem::Impl
    {

    public:

	virtual unsigned long long min_size() const override { return 256 * MiB; }
	virtual unsigned long long max_size() const override { return 16 * EiB - 1 * B; }

	virtual bool supports_mounted_shrink() const override { return true; }
	virtual bool supports_mounted_grow() const override { return true; }

	virtual bool supports_unmounted_shrink() const override { return false; }
	virtual bool supports_unmounted_grow() const override { return false; }

	virtual bool supports_label() const override { return true; }
	virtual bool supports_modify_label() const override { return true; }
	virtual unsigned int max_labelsize() const override { return 256; }

	virtual bool supports_uuid() const override { return true; }

	BtrfsRaidLevel get_metadata_raid_level() const { return metadata_raid_level; }
	void set_metadata_raid_level(BtrfsRaidLevel metadata_raid_level) { Impl::metadata_raid_level = metadata_raid_level; }

	BtrfsRaidLevel get_data_raid_level() const { return data_raid_level; }
	void set_data_raid_level(BtrfsRaidLevel data_raid_level) { Impl::data_raid_level = data_raid_level; }

	vector<BtrfsRaidLevel> get_allowed_metadata_raid_levels() const;
	vector<BtrfsRaidLevel> get_allowed_data_raid_levels() const;

	bool has_quota() const { return quota; }
	void set_quota(bool quota);

	FilesystemUser* add_device(BlkDevice* blk_device);
	void remove_device(BlkDevice* blk_device);

	BtrfsSubvolume* get_top_level_btrfs_subvolume();
	const BtrfsSubvolume* get_top_level_btrfs_subvolume() const;

	BtrfsSubvolume* get_default_btrfs_subvolume();
	const BtrfsSubvolume* get_default_btrfs_subvolume() const;

	void set_default_btrfs_subvolume(BtrfsSubvolume* btrfs_subvolume) const;

	vector<BtrfsSubvolume*> get_btrfs_subvolumes();
	vector<const BtrfsSubvolume*> get_btrfs_subvolumes() const;

	virtual bool predicate_etc_fstab(const FstabEntry* fstab_entry) const override;

	virtual bool predicate_proc_mounts(const FstabEntry* fstab_entry) const override;

	virtual const BlkDevice* get_etc_fstab_blk_device(const MountPoint* mount_point) const override;

	BtrfsSubvolume* find_btrfs_subvolume_by_path(const string& path);
	const BtrfsSubvolume* find_btrfs_subvolume_by_path(const string& path) const;

	BtrfsQgroup* create_btrfs_qgroup(const BtrfsQgroup::id_t& id);

	vector<BtrfsQgroup*> get_btrfs_qgroups();
	vector<const BtrfsQgroup*> get_btrfs_qgroups() const;

	BtrfsQgroup* find_btrfs_qgroup_by_id(const BtrfsQgroup::id_t& id);
	const BtrfsQgroup* find_btrfs_qgroup_by_id(const BtrfsQgroup::id_t& id) const;

        bool get_configure_snapper() const { return configure_snapper; }
        void set_configure_snapper(bool configure) { Impl::configure_snapper = configure; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

    public:

	Impl();

	virtual ~Impl();

	Impl(const xmlNode* node);

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual FsType get_type() const override { return FsType::BTRFS; }

	virtual const char* get_classname() const override { return DeviceTraits<Btrfs>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return "btrfs"; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	static void probe_btrfses(Prober& prober);
	virtual void probe_pass_2a(Prober& prober) override;
	virtual void probe_pass_2b(Prober& prober) override;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;
	virtual ResizeInfo detect_resize_info_on_disk(const BlkDevice* blk_device = nullptr) const override;

	uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	virtual uf_t used_features_pure() const override { return UF_BTRFS; }

	virtual Btrfs* get_non_impl() override { return to_btrfs(Device::Impl::get_non_impl()); }
	virtual const Btrfs* get_non_impl() const override { return to_btrfs(Device::Impl::get_non_impl()); }

	virtual void do_create() override;

	virtual void do_resize(const CommitData& commit_data, const Action::Resize* action) const override;

	virtual void do_mount(CommitData& commit_data, const CommitOptions& commit_options, MountPoint* mount_point) const override;

	virtual void do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const override;

	virtual void do_set_label() const override;

	virtual void do_pre_mount() const override;

	virtual Text do_reallot_text(const CommitData& commit_data, const Action::Reallot* action) const override;
	virtual void do_reallot(const CommitData& commit_data, const Action::Reallot* action) const override;

	virtual Text do_set_quota_text(const CommitData& commit_data, const Action::SetQuota* action) const;
	virtual void do_set_quota(const CommitData& commit_data, const Action::SetQuota* action) const;
	virtual uf_t do_set_quota_used_features() const { return UF_BTRFS; }

	void parse_mkfs_output(const vector<string>& stdout);

	void do_reduce(const BlkDevice* blk_device) const;
	void do_extend(const BlkDevice* blk_device) const;

	virtual void add_dependencies(Actiongraph::Impl& actiongraph) const override;

    private:

	bool configure_snapper = false;
	SnapperConfig* snapper_config = nullptr;

	BtrfsRaidLevel metadata_raid_level = BtrfsRaidLevel::UNKNOWN;
	BtrfsRaidLevel data_raid_level = BtrfsRaidLevel::UNKNOWN;

	bool quota = false;

	/**
	 * mutable to allow updating cache from const functions. Otherwise
	 * caching would not be possible when working with the probed
	 * devicegraph.
	 */
	mutable CDgD<ResizeInfo> multi_device_resize_info;

    };

}

#endif
