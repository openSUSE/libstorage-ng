/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


namespace storage
{

    using namespace std;

    class EtcFstab;


    template <> struct DeviceTraits<BlkFilesystem> { static const char* classname; };


    // abstract class

    class BlkFilesystem::Impl : public Filesystem::Impl
    {
    public:

	virtual bool supports_label() const = 0;
	virtual unsigned int max_labelsize() const = 0;

	const string& get_label() const { return label; }
	void set_label(const string& label);

	virtual bool supports_uuid() const = 0;

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid);

	virtual bool supports_external_journal() const { return false; }

	const string& get_mkfs_options() const { return mkfs_options; }
	void set_mkfs_options(const string& mkfs_options);

	const string& get_tune_options() const { return tune_options; }
	void set_tune_options(const string& tune_options);

	virtual MountByType get_default_mount_by() const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	virtual void probe_pass_3(Devicegraph* probed, SystemInfo& systeminfo);

	vector<const BlkDevice*> get_blk_devices() const;
	const BlkDevice* get_blk_device() const;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual ResizeInfo detect_resize_info() const override;
	virtual ResizeInfo detect_resize_info_pure() const;
	void set_resize_info(const ResizeInfo& resize_info);

	virtual ContentInfo detect_content_info() const;
	virtual ContentInfo detect_content_info_pure() const;
	void set_content_info(const ContentInfo& content_info);

	virtual string get_mount_name() const override;
	virtual string get_mount_by_name() const override;

	virtual BlkFilesystem* get_non_impl() override { return to_blk_filesystem(Device::Impl::get_non_impl()); }
	virtual const BlkFilesystem* get_non_impl() const override { return to_blk_filesystem(Device::Impl::get_non_impl()); }

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_set_label_text(Tense tense) const;
	virtual void do_set_label() const;

	virtual Text do_set_uuid_text(Tense tense) const;
	virtual void do_set_uuid() const;

	virtual Text do_rename_in_etc_fstab_text(const Device* lhs, Tense tense) const;
	virtual void do_rename_in_etc_fstab(CommitData& commit_data, const Device* lhs) const;

	virtual Text do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				    Tense tense) const override;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

    protected:

	Impl()
	    : Filesystem::Impl(), label(), uuid(), mkfs_options(), tune_options(),
	      resize_info(), content_info()
	    {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

	virtual void probe_uuid();

	static bool detect_is_windows(const string& mountpoint);
	static bool detect_is_efi(const string& mountpoint);
	static unsigned detect_num_homes(const string& mountpoint);

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


    namespace Action
    {

	class SetLabel : public Modify
	{
	public:

	    SetLabel(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	};


	class SetUuid : public Modify
	{
	public:

	    SetUuid(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	};


	class RenameInEtcFstab : public RenameIn
	{
	public:

	    RenameInEtcFstab(sid_t sid)	: RenameIn(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	    virtual const BlkDevice* get_renamed_blk_device(const Actiongraph::Impl& actiongraph,
							    Side side) const override;

	};

    }

}

#endif
