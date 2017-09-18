/*
 * Copyright (c) 2015 Novell, Inc.
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


#ifndef STORAGE_BTRFS_IMPL_H
#define STORAGE_BTRFS_IMPL_H


#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Utils/SnapperConfig.h"
#include "storage/Action.h"
#include "storage/Utils/HumanString.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Btrfs> { static const char* classname; };


    class Btrfs::Impl : public BlkFilesystem::Impl
    {

    public:

	virtual unsigned long long min_size() const override { return 256 * MiB; }
	virtual unsigned long long max_size() const override { return 16 * EiB - 1 * B; }

	virtual bool supports_shrink() const override { return true; }
	virtual bool supports_grow() const override { return true; }

	virtual bool supports_label() const override { return true; }
	virtual unsigned int max_labelsize() const override { return 256; }

	virtual bool supports_uuid() const override { return true; }

	BtrfsSubvolume* get_top_level_btrfs_subvolume();
	const BtrfsSubvolume* get_top_level_btrfs_subvolume() const;

	BtrfsSubvolume* get_default_btrfs_subvolume();
	const BtrfsSubvolume* get_default_btrfs_subvolume() const;

	void set_default_btrfs_subvolume(BtrfsSubvolume* btrfs_subvolume) const;

	vector<BtrfsSubvolume*> get_btrfs_subvolumes();
	vector<const BtrfsSubvolume*> get_btrfs_subvolumes() const;

	virtual FstabEntry* find_etc_fstab_entry(EtcFstab& etc_fstab, const vector<string>& names) const override;
	virtual const FstabEntry* find_etc_fstab_entry(const EtcFstab& etc_fstab, const vector<string>& names) const override;

	BtrfsSubvolume* find_btrfs_subvolume_by_path(const string& path);
	const BtrfsSubvolume* find_btrfs_subvolume_by_path(const string& path) const;

        bool get_configure_snapper() const { return configure_snapper; }
        void set_configure_snapper(bool configure) { Impl::configure_snapper = configure; }

    public:

	Impl();

        virtual ~Impl();

	Impl(const xmlNode* node);

	virtual void check() const override;

	virtual FsType get_type() const override { return FsType::BTRFS; }

	virtual const char* get_classname() const override { return DeviceTraits<Btrfs>::classname; }

	virtual string get_displayname() const override { return "btrfs"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void probe_pass_2(Prober& prober) override;

	virtual uint64_t used_features() const override;

	virtual void do_create() override;

	virtual void do_resize(ResizeMode resize_mode, const Device* rhs) const override;

	virtual void do_mount(CommitData& commit_data, const MountPoint* mount_point) const override;

	virtual void do_add_to_etc_fstab(CommitData& commit_data, const MountPoint* mount_point) const override;

	virtual void do_set_label() const override;

    private:

        bool configure_snapper;
        SnapperConfig * snapper_config;
    };

}

#endif
