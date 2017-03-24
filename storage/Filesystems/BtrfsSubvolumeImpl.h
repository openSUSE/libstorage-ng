/*
 * Copyright (c) 2017 SUSE LLC
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
	    : Mountable::Impl(), id(unknown_id), path(path), default_btrfs_subvolume(false), nocow(false) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<BtrfsSubvolume>::classname; }

	virtual string get_displayname() const override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void probe_pass_3(Devicegraph* probed, SystemInfo& systeminfo, const EtcFstab& fstab,
				  const string& mountpoint);

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

	virtual FsType get_mount_type() const override { return FsType::BTRFS; }

	virtual string get_mount_name() const override;
	virtual string get_mount_by_name() const override;
	virtual vector<string> get_mount_options() const override;

	virtual FstabEntry* find_etc_fstab_entry(EtcFstab& etc_fstab, const vector<string>& names) const override;
	virtual const FstabEntry* find_etc_fstab_entry(const EtcFstab& etc_fstab, const vector<string>& names) const override;

	BtrfsSubvolume* create_btrfs_subvolume(const string& path);

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() const override;

	virtual Text do_mount_text(const string& mountpoint, Tense tense) const;

        virtual Text do_umount_text(const string& mountpoint, Tense tense) const;

        virtual Text do_add_to_etc_fstab_text(const string& mountpoint, Tense tense) const;

        virtual Text do_remove_from_etc_fstab_text(const string& mountpoint, Tense tense) const;

	virtual Text do_set_nocow_text(Tense tense) const;
	virtual void do_set_nocow() const;

	virtual Text do_set_default_btrfs_subvolume_text(Tense tense) const;
	virtual void do_set_default_btrfs_subvolume() const;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

    protected:

	void save(xmlNode* node) const override;

    private:

	long id;
	string path;

	bool default_btrfs_subvolume;
	bool nocow;

    };


    namespace Action
    {

	class SetNocow : public Modify
	{
	public:

	    SetNocow(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	};


	class SetDefaultBtrfsSubvolume : public Modify
	{
	public:

	    SetDefaultBtrfsSubvolume(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	};

    }

}

#endif
