/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#ifndef STORAGE_BTRFSSUBVOLUME_IMPL_H
#define STORAGE_BTRFSSUBVOLUME_IMPL_H


#include "storage/Filesystems/BtrfsSubvolume.h"
#include "storage/Filesystems/BtrfsQgroup.h"
#include "storage/Filesystems/MountableImpl.h"


namespace storage
{

    using namespace std;


    class EtcFstab;


    template <> struct DeviceTraits<BtrfsSubvolume> { static const char* classname; };


    class BtrfsSubvolume::Impl : public Mountable::Impl
    {
    public:

	static const long top_level_id = 5;
	static const long unknown_id = -1;

	Impl(const string& path)
	    : Mountable::Impl(), path(path) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<BtrfsSubvolume>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual void probe_pass_2a(Prober& prober, const string& mount_point);
	virtual void probe_pass_2b(Prober& prober, const string& mount_point);

	long get_id() const { return id; }
	void set_id(long id) { Impl::id = id; }

	bool is_top_level() const { return id == top_level_id; }

	const string& get_path() const { return path; }
	void set_path(const string& path) { Impl::path = path; }

	bool is_default_btrfs_subvolume() const { return default_btrfs_subvolume; }
	void set_default_btrfs_subvolume();

	bool is_nocow() const { return nocow; }
	void set_nocow(bool nocow) { Impl::nocow = nocow; }

	Btrfs* get_btrfs();
	const Btrfs* get_btrfs() const;

	BtrfsSubvolume* get_top_level_btrfs_subvolume();
	const BtrfsSubvolume* get_top_level_btrfs_subvolume() const;

	virtual Filesystem* get_filesystem() override;
	virtual const Filesystem* get_filesystem() const override;

	bool has_snapshots() const;

	vector<BtrfsSubvolume*> get_snapshots();
	vector<const BtrfsSubvolume*> get_snapshots() const;

	bool has_origin() const;

	BtrfsSubvolume* get_origin();
	const BtrfsSubvolume* get_origin() const;

	/**
	 * Get the id of the corresponding btrfs qgroup. Can be BtrfsQgroup::Impl::unknown_id.
	 */
	BtrfsQgroup::id_t get_btrfs_qgroup_id() const;

	bool has_btrfs_qgroup() const;

	BtrfsQgroup* get_btrfs_qgroup();
	const BtrfsQgroup* get_btrfs_qgroup() const;

	BtrfsQgroup* create_btrfs_qgroup();

	virtual FsType get_default_mount_type() const override { return FsType::BTRFS; }

	virtual Text get_message_name() const override;

	virtual string get_mount_name() const override;
	virtual string get_mount_by_name(const MountPoint* mount_point) const override;
	virtual vector<string> get_mount_options() const override;

	virtual MountByType get_default_mount_by() const override;
	virtual MountOpts default_mount_options() const override;

	virtual vector<ExtendedFstabEntry> find_etc_fstab_entries_unfiltered(SystemInfo::Impl& system_info) const override;

	virtual vector<FstabEntry*> find_etc_fstab_entries_unfiltered(EtcFstab& etc_fstab,
								      const FstabAnchor& fstab_anchor) const override;

	virtual bool predicate_etc_fstab(const FstabEntry* fstab_entry) const override;

	virtual vector<ExtendedFstabEntry> find_proc_mounts_entries_unfiltered(SystemInfo::Impl& system_info) const override;

	virtual bool predicate_proc_mounts(const FstabEntry* fstab_entry) const override;

	BtrfsSubvolume* create_btrfs_subvolume(const string& path);

	virtual uf_t used_features_pure() const override { return UF_BTRFS; }

	virtual BtrfsSubvolume* get_non_impl() override { return to_btrfs_subvolume(Device::Impl::get_non_impl()); }
	virtual const BtrfsSubvolume* get_non_impl() const override { return to_btrfs_subvolume(Device::Impl::get_non_impl()); }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_mount_text(const MountPoint* mount_point, Tense tense) const override;

	virtual Text do_unmount_text(const MountPoint* mount_point, Tense tense) const override;

	virtual Text do_add_to_etc_fstab_text(const MountPoint* mount_point, Tense tense) const override;

	virtual Text do_remove_from_etc_fstab_text(const MountPoint* mount_point, Tense tense) const override;

	virtual Text do_set_nocow_text(Tense tense) const;
	virtual void do_set_nocow() const;
	virtual uf_t do_set_nocow_used_features() const { return used_features_pure(); }

	virtual Text do_set_default_btrfs_subvolume_text(Tense tense) const;
	virtual void do_set_default_btrfs_subvolume() const;
	virtual uf_t do_set_default_btrfs_subvolume_used_features() const { return used_features_pure(); }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return used_features_pure(); }

	virtual void do_pre_mount() const override;

    protected:

	virtual void save(xmlNode* node) const override;

	void probe_id(const string& mount_point);

    private:

	long id = unknown_id;
	string path;

	bool default_btrfs_subvolume = false;
	bool nocow = false;

    };

}

#endif
