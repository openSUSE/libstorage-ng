/*
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_TMPFS_IMPL_H
#define STORAGE_TMPFS_IMPL_H


#include "storage/Filesystems/Tmpfs.h"
#include "storage/Filesystems/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Tmpfs> { static const char* classname; };


    class Tmpfs::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return FsType::TMPFS; }

	virtual const char* get_classname() const override { return DeviceTraits<Tmpfs>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return "tmpfs"; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	static void probe_tmpfses(Prober& prober);

	virtual uf_t used_features_pure() const override { return 0; }

	virtual Text get_message_name() const override;

	virtual string get_mount_name() const override;
	virtual string get_mount_by_name(const MountPoint* mount_point) const override;

	virtual MountByType get_default_mount_by() const override;

	virtual vector<MountByType> possible_mount_bys() const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual vector<ExtendedFstabEntry> find_proc_mounts_entries_unfiltered(SystemInfo::Impl& system_info) const override;

    protected:

	virtual void save(xmlNode* node) const override;

    };

}

#endif
