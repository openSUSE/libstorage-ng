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
#include "storage/Utils/FileUtils.h"
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

	virtual FsType get_type() const = 0;

	const string& get_label() const { return label; }
	void set_label(const string& label);

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid);

	const string& get_mkfs_options() const { return mkfs_options; }
	void set_mkfs_options(const string& mkfs_options);

	const string& get_tune_options() const { return tune_options; }
	void set_tune_options(const string& tune_options);

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual void probe_pass_3(Devicegraph* probed, SystemInfo& systeminfo, EtcFstab& fstab);

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

	string get_mount_by_string() const;

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_set_label_text(Tense tense) const;
	virtual void do_set_label() const;

	virtual Text do_set_uuid_text(Tense tense) const;
	virtual void do_set_uuid() const;

	virtual Text do_mount_text(const string& mountpoint, Tense tense) const override;
	virtual void do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const override;

	virtual Text do_umount_text(const string& mountpoint, Tense tense) const override;
	virtual void do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const override;

	virtual Text do_add_to_etc_fstab_text(const string& mountpoint, Tense tense) const override;
	virtual void do_add_to_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const override;

	virtual Text do_rename_in_etc_fstab_text(const Device* lhs, const string& mountpoint,Tense tense) const;
	virtual void do_rename_in_etc_fstab(const Actiongraph::Impl& actiongraph, const Device* lhs, const string& mountpoint) const;

	virtual Text do_remove_from_etc_fstab_text(const string& mountpoint, Tense tense) const override;
	virtual void do_remove_from_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const override;

	virtual Text do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				    Tense tense) const override;

    protected:

	Impl()
	    : Filesystem::Impl(), label(), uuid(), mkfs_options(), tune_options(),
	    resize_info(), content_info()
	    {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

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


    namespace Action
    {

	class SetLabel : public Modify
	{
	public:

	    SetLabel(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};


	class SetUuid : public Modify
	{
	public:

	    SetUuid(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};


	class RenameInEtcFstab : public RenameIn
	{
	public:

	    RenameInEtcFstab(sid_t sid, const string& mountpoint)
		: RenameIn(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual const BlkDevice* get_renamed_blk_device(const Actiongraph::Impl& actiongraph,
							    Side side) const override;

	    const string mountpoint;

	};

    }


    class EnsureMounted : boost::noncopyable
    {

    public:

	/**
	 * Ensures that the blk filesystem is mounted somewhere.
	 *
	 * The mode is not enforced.
	 */
	EnsureMounted(const BlkFilesystem::Impl* blk_filesystem);

	/**
	 * Returns any mountpoint of the blk_filesystem.
	 */
	string get_any_mountpoint() const;

    private:

	const BlkFilesystem::Impl* blk_filesystem;

	unique_ptr<TmpMount> tmp_mount;

    };

}

#endif
