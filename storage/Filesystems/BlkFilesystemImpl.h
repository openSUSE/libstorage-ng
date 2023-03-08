/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2022] SUSE LLC
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


#ifndef STORAGE_BLK_FILESYSTEM_IMPL_H
#define STORAGE_BLK_FILESYSTEM_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/CDgD.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/FreeInfo.h"
#include "storage/Actions/ResizeImpl.h"


namespace storage
{

    using namespace std;

    class EtcFstab;


    template <> struct DeviceTraits<BlkFilesystem> { static const char* classname; };


    // abstract class

    class BlkFilesystem::Impl : public Filesystem::Impl
    {
    public:

	virtual unsigned long long min_size() const = 0;
	virtual unsigned long long max_size() const = 0;

	bool supports_shrink() const { return supports_mounted_shrink() || supports_unmounted_shrink(); }
	bool supports_grow() const { return supports_mounted_grow() || supports_unmounted_grow(); }

	bool supports_mounted_resize(ResizeMode resize_mode) const;
	bool supports_unmounted_resize(ResizeMode resize_mode) const;

	virtual bool supports_mounted_shrink() const = 0;
	virtual bool supports_mounted_grow() const = 0;

	virtual bool supports_unmounted_shrink() const = 0;
	virtual bool supports_unmounted_grow() const = 0;

	virtual bool supports_label() const = 0;
	virtual bool supports_modify_label() const { return false; }
	virtual unsigned int max_labelsize() const = 0;

	const string& get_label() const { return label; }
	void set_label(const string& label) { Impl::label = label; }

	virtual bool supports_uuid() const = 0;
	virtual bool supports_modify_uuid() const { return false; }

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	virtual bool supports_external_journal() const { return false; }

	const string& get_mkfs_options() const { return mkfs_options; }
	void set_mkfs_options(const string& mkfs_options) { Impl::mkfs_options = mkfs_options; }

	virtual bool supports_tune_options() const { return false; }

	const string& get_tune_options() const { return tune_options; }
	void set_tune_options(const string& tune_options) { Impl::tune_options = tune_options; }

	virtual MountByType get_default_mount_by() const override;

	virtual vector<MountByType> possible_mount_bys() const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	static void probe_blk_filesystems(Prober& prober);
	virtual void probe_pass_2a(Prober& prober);

	vector<BlkDevice*> get_blk_devices();
	vector<const BlkDevice*> get_blk_devices() const;

	const BlkDevice* get_blk_device() const;

	virtual void wait_for_devices() const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;
	virtual ResizeInfo detect_resize_info_on_disk(const BlkDevice* blk_device = nullptr) const;
	void set_resize_info(const ResizeInfo& resize_info);

	unsigned long long used_size_on_disk() const;
	unsigned long long free_size_on_disk() const;

	virtual ContentInfo detect_content_info() const;
	virtual ContentInfo detect_content_info_on_disk() const;
	void set_content_info(const ContentInfo& content_info);

	virtual Text get_message_name() const override;

	virtual string get_mount_name() const override;
	virtual string get_mount_by_name(const MountPoint* mount_point) const override;

	bool spec_match(const string& spec) const;

	/**
	 * Returns the blk device used in /etc/fstab (based on the
	 * FstabAnchor information from mount_point). If that fails
	 * returns any blk device of the filesystem.
	 */
	virtual const BlkDevice* get_etc_fstab_blk_device(const MountPoint* mount_point) const;

	virtual vector<ExtendedFstabEntry> find_etc_fstab_entries_unfiltered(SystemInfo::Impl& system_info) const override;

	virtual vector<ExtendedFstabEntry> find_proc_mounts_entries_unfiltered(SystemInfo::Impl& system_info) const override;

	virtual BlkFilesystem* get_non_impl() override { return to_blk_filesystem(Device::Impl::get_non_impl()); }
	virtual const BlkFilesystem* get_non_impl() const override { return to_blk_filesystem(Device::Impl::get_non_impl()); }

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_set_label_text(Tense tense) const;
	virtual void do_set_label() const;
	virtual uf_t do_set_label_used_features() const { return used_features_pure(); }

	virtual Text do_set_uuid_text(Tense tense) const;
	virtual void do_set_uuid() const;
	virtual uf_t do_set_uuid_used_features() const { return used_features_pure(); }

	virtual Text do_set_tune_options_text(Tense tense) const;
	virtual void do_set_tune_options() const;
	virtual uf_t do_set_tune_options_used_features() const { return used_features_pure(); }

	virtual Text do_resize_text(const CommitData& commit_data, const Action::Resize* action) const override;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

    protected:

	Impl()
	    : Filesystem::Impl(), label(), uuid(), mkfs_options(), tune_options(),
	      resize_info(), content_info()
	    {}

	Impl(const xmlNode* node);

	virtual void save(xmlNode* node) const override;

	virtual void probe_uuid();

	static bool detect_is_windows(const string& mount_point);
	static bool detect_is_efi(const string& mount_point);
	static unsigned detect_num_homes(const string& mount_point);

	/**
	 * Checks whether the filesystem is permanent. A non permanent
	 * filesystem cannot use mount-by uuid or label.
	 *
	 * E.g. a swap located on a plain encryption with the swap
	 * option set in /etc/crypttab is not permanent since it is
	 * regenerated at every boot.
	 */
	virtual bool is_permanent() const { return true; }

    private:

	string label;
	string uuid;

	string mkfs_options;
	string tune_options;

	/**
	 * mutable to allow updating cache from const functions. Otherwise
	 * caching would not be possible when working with the probed
	 * devicegraph.
	 */
	mutable CDgD<ResizeInfo> resize_info;
	mutable CDgD<ContentInfo> content_info;

    };


    static_assert(std::is_abstract<BlkFilesystem>(), "BlkFilesystem ought to be abstract.");
    static_assert(std::is_abstract<BlkFilesystem::Impl>(), "BlkFilesystem::Impl ought to be abstract.");

}

#endif
